# Refactoring Recommendations (P2 Tasks)

**Date:** November 4, 2025  
**Priority:** P2 (Long-term)  
**Purpose:** Option 7 - Architecture Improvements

---

## Overview

This document outlines long-term refactoring opportunities to improve the notification interface architecture, type safety, and maintainability.

## Current Architecture Assessment

### Strengths ✅
- Clean separation of concerns
- Minimal interface in kernel
- Flexible implementation in node
- Low coupling
- Good performance

### Areas for Improvement ⚠️
- Static casting required for extended functionality
- No compile-time verification of implementation type
- Limited extensibility
- No capability discovery

---

## Refactoring Option 1: Unified Interface

### Proposal
Move `warningSet` and `warningUnset` to the base `kernel::Notifications` interface.

### Implementation
```cpp
// kernel/notifications_interface.h
namespace kernel {
    enum class Warning {
        UNKNOWN_NEW_RULES_ACTIVATED,
        LARGE_WORK_INVALID_CHAIN,
    };

    class Notifications {
    public:
        // Existing methods...
        virtual void warning(const bilingual_str& warning) {}
        
        // NEW: Add typed warning management
        virtual void warningSet(Warning id, const bilingual_str& message) {}
        virtual void warningUnset(Warning id) {}
        
        virtual void flushError(const std::string& debug_message) {}
        virtual void fatalError(const std::string& debug_message, 
                               const bilingual_str& user_message = {}) {}
    };
}
```

### Pros ✅
- No casting required
- Type-safe by default
- Cleaner call sites
- Easier to use

### Cons ❌
- Adds kernel dependency on Warning enum
- Broader interface (less minimal)
- All implementations must handle warnings

### Recommendation
**CONSIDER** - Good balance of usability and design

---

## Refactoring Option 2: Capability Pattern

### Proposal
Use capability queries to determine available functionality.

### Implementation
```cpp
// kernel/notifications_interface.h
namespace kernel {
    enum class Capability {
        TYPED_WARNINGS,
        PROGRESS_REPORTING,
        DETAILED_ERRORS,
    };

    class Notifications {
    public:
        virtual bool hasCapability(Capability cap) const { return false; }
        
        template<typename T>
        T* as() {
            return dynamic_cast<T*>(this);
        }
        
        // Existing methods...
    };
}

// Usage
auto* notif = &GetNotifications();
if (notif->hasCapability(Capability::TYPED_WARNINGS)) {
    if (auto* kn = notif->as<node::KernelNotifications>()) {
        kn->warningSet(...);
    }
}
```

### Pros ✅
- Runtime capability discovery
- Flexible extensibility
- Type-safe with dynamic_cast
- Clean abstraction

### Cons ❌
- More complex
- Runtime overhead (minimal)
- Verbose usage

### Recommendation
**FUTURE** - Good for plugin architecture

---

## Refactoring Option 3: CRTP Pattern

### Proposal
Use Curiously Recurring Template Pattern for type-safe derivation.

### Implementation
```cpp
// kernel/notifications_interface.h
template<typename Derived>
class NotificationsBase {
public:
    void warningSet(Warning id, const bilingual_str& message) {
        static_cast<Derived*>(this)->warningSetImpl(id, message);
    }
    
protected:
    void warningSetImpl(Warning id, const bilingual_str& message) {
        // Default implementation
    }
};

// node/kernel_notifications.h
class KernelNotifications : public NotificationsBase<KernelNotifications> {
public:
    void warningSetImpl(Warning id, const bilingual_str& message) {
        // Actual implementation
    }
};
```

### Pros ✅
- Compile-time polymorphism
- Zero runtime overhead
- Type-safe
- No virtual calls

### Cons ❌
- Template complexity
- Header dependencies
- Harder to understand
- Less flexible

### Recommendation
**NOT RECOMMENDED** - Too complex for this use case

---

## Refactoring Option 4: Visitor Pattern

### Proposal
Use visitor pattern for extensible operations.

### Implementation
```cpp
// kernel/notifications_interface.h
class NotificationVisitor {
public:
    virtual void visitWarningSet(Warning id, const bilingual_str& msg) = 0;
    virtual void visitWarningUnset(Warning id) = 0;
    virtual void visitFatalError(const std::string& debug, 
                                const bilingual_str& user) = 0;
};

class Notifications {
public:
    virtual void accept(NotificationVisitor& visitor) = 0;
};
```

### Pros ✅
- Highly extensible
- Separates operations from structure
- Easy to add new operations

### Cons ❌
- Over-engineered for current needs
- Complex implementation
- More boilerplate
- Harder to understand

### Recommendation
**NOT RECOMMENDED** - Too complex

---

## Refactoring Option 5: Type-Safe Casting Helper

### Proposal
Provide helper function for safe casting with runtime checks.

### Implementation
```cpp
// kernel/notifications_interface.h
namespace kernel {
    template<typename T>
    T* notification_cast(Notifications* notif) {
        T* result = dynamic_cast<T*>(notif);
        if (!result) {
            // Log warning or throw
            LogPrintf("Warning: notification_cast failed\n");
        }
        return result;
    }
}

// Usage
if (auto* kn = kernel::notification_cast<node::KernelNotifications>(
        &GetNotifications())) {
    kn->warningSet(...);
}
```

### Pros ✅
- Runtime safety
- Clear intent
- Minimal changes
- Easy to adopt

### Cons ❌
- Runtime overhead (minimal)
- Still requires casting
- Not fully type-safe

### Recommendation
**RECOMMENDED** - Good incremental improvement

---

## Type Safety Improvements

### Current Issues
1. Static cast assumes type without verification
2. No compile-time enforcement
3. Potential for misuse

### Proposed Solutions

#### Solution A: Assert in Debug Builds
```cpp
#ifdef DEBUG
#define KERNEL_NOTIF_CAST(notif) \
    (assert(dynamic_cast<node::KernelNotifications*>(&notif) != nullptr), \
     static_cast<node::KernelNotifications&>(notif))
#else
#define KERNEL_NOTIF_CAST(notif) \
    static_cast<node::KernelNotifications&>(notif)
#endif
```

#### Solution B: Smart Pointer Wrapper
```cpp
template<typename Base, typename Derived>
class NotificationPtr {
    Base* ptr;
public:
    NotificationPtr(Base* p) : ptr(p) {
        assert(dynamic_cast<Derived*>(p) != nullptr);
    }
    
    Derived* operator->() {
        return static_cast<Derived*>(ptr);
    }
};
```

---

## Interface Hierarchy Improvements

### Current Structure
```
kernel::Notifications (base)
    └── node::KernelNotifications (adds methods)
```

### Proposed Alternative 1: Layered Interfaces
```
kernel::Notifications (core)
    ├── kernel::WarningNotifications (warning management)
    └── kernel::ProgressNotifications (progress reporting)
        └── node::KernelNotifications (implements all)
```

### Proposed Alternative 2: Composition
```cpp
class Notifications {
    std::unique_ptr<WarningManager> warnings;
    std::unique_ptr<ErrorManager> errors;
    std::unique_ptr<ProgressReporter> progress;
};
```

---

## Recommended Refactoring Plan

### Phase 1: Immediate (Low Risk)
1. ✅ Add inline documentation (DONE)
2. ✅ Add API reference (DONE)
3. ✅ Create migration guide (DONE)
4. ⬜ Add notification_cast helper
5. ⬜ Add debug assertions

### Phase 2: Short-term (Medium Risk)
1. ⬜ Move Warning enum to kernel
2. ⬜ Add warningSet/Unset to base interface
3. ⬜ Update all call sites
4. ⬜ Add capability queries (optional)

### Phase 3: Long-term (Higher Risk)
1. ⬜ Consider interface restructuring
2. ⬜ Evaluate composition pattern
3. ⬜ Assess plugin requirements
4. ⬜ Implement chosen architecture

---

## Code Examples

### Before Refactoring
```cpp
// Current approach - requires casting
static_cast<node::KernelNotifications&>(GetNotifications())
    .warningSet(Warning::LARGE_WORK_INVALID_CHAIN, message);
```

### After Refactoring (Option 1)
```cpp
// Unified interface - no casting
GetNotifications().warningSet(
    Warning::LARGE_WORK_INVALID_CHAIN, message);
```

### After Refactoring (Option 5)
```cpp
// Safe cast helper
if (auto* kn = kernel::notification_cast<node::KernelNotifications>(
        &GetNotifications())) {
    kn->warningSet(Warning::LARGE_WORK_INVALID_CHAIN, message);
}
```

---

## Impact Assessment

### Refactoring Option 1 (Unified Interface)
- **Effort:** Medium (2-3 days)
- **Risk:** Low
- **Benefit:** High (cleaner code)
- **Breaking Changes:** Yes (interface change)

### Refactoring Option 5 (Cast Helper)
- **Effort:** Low (4-6 hours)
- **Risk:** Very Low
- **Benefit:** Medium (safer code)
- **Breaking Changes:** No

---

## Migration Strategy

### Step 1: Prepare
- Document current usage patterns
- Identify all call sites
- Create test suite
- Establish baseline metrics

### Step 2: Implement
- Add new interface/helpers
- Update implementation
- Migrate call sites incrementally
- Test each change

### Step 3: Verify
- Run full test suite
- Performance benchmarks
- Code review
- Documentation update

### Step 4: Deploy
- Staged rollout
- Monitor for issues
- Gather feedback
- Iterate as needed

---

## Conclusions

### Recommended Approach

**Short-term (Next Sprint):**
- Implement Option 5 (cast helper) ✅
- Add debug assertions ✅
- No breaking changes

**Medium-term (Next Quarter):**
- Implement Option 1 (unified interface) if team agrees
- Coordinate with stakeholders
- Plan migration

**Long-term (Next Year):**
- Evaluate architecture based on real-world usage
- Consider plugin requirements
- Implement comprehensive refactoring if needed

### Priority

**Priority:** P2 (Important but not urgent)  
**Risk:** Low to Medium  
**Value:** High (improved maintainability)  

---

**Status:** Option 7 Complete ✅  
**Next Steps:** Discuss with team, prioritize changes
