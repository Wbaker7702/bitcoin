# Notification Interface Developer Guide

**Version:** 1.0  
**Date:** November 4, 2025  
**Audience:** Bitcoin Core Developers

---

## Overview

The Bitcoin Core notification interface provides a clean abstraction for handling system events, warnings, and errors. This guide covers the current implementation and best practices.

## Architecture

### Interface Hierarchy

```
kernel::Notifications (base interface)
    └── node::KernelNotifications (implementation)
```

### Base Interface: `kernel::Notifications`

Location: `src/kernel/notifications_interface.h`

Methods:
- `blockTip()` - Called when a new block tip is set
- `headerTip()` - Called when header sync progresses
- `progress()` - Reports progress for long operations
- `warning()` - Generic warning notification
- `flushError()` - Error during flush operations
- `fatalError()` - Unrecoverable errors

### Implementation: `node::KernelNotifications`

Location: `src/node/kernel_notifications.h`

Additional methods (not in base):
- `warningSet()` - Set a specific warning
- `warningUnset()` - Clear a specific warning

## API Reference

### fatalError()

**Signature:**
```cpp
virtual void fatalError(const std::string& debug_message, 
                       const bilingual_str& user_message = {})
```

**Purpose:** Notify about unrecoverable errors

**Parameters:**
- `debug_message` - English-only message for logs/debugging
- `user_message` - Translatable message for end users (optional)

**Example:**
```cpp
bilingual_str error = _("Failed to connect to peer");
notifications.fatalError(error.original, error);
```

---

### flushError()

**Signature:**
```cpp
virtual void flushError(const std::string& debug_message)
```

**Purpose:** Notify about disk flush errors (non-fatal)

**Parameters:**
- `debug_message` - English-only debug message

**Example:**
```cpp
notifications.flushError("Failed to flush block data to disk");
```

---

### warning()

**Signature:**
```cpp
virtual void warning(const bilingual_str& warning)
```

**Purpose:** Generic warning notification

**Example:**
```cpp
notifications.warning(_("Low disk space"));
```

---

### warningSet() / warningUnset()

**Note:** These are specific to `node::KernelNotifications`

**Signature:**
```cpp
void warningSet(kernel::Warning id, const bilingual_str& message);
void warningUnset(kernel::Warning id);
```

**Purpose:** Manage typed warnings

**Usage:**
```cpp
// Must cast to KernelNotifications
auto& kernel_notif = static_cast<node::KernelNotifications&>(
    GetNotifications()
);
kernel_notif.warningSet(
    kernel::Warning::LARGE_WORK_INVALID_CHAIN,
    _("Warning message")
);
```

---

## Common Patterns

### Pattern 1: Fatal Error with User Message

```cpp
// Create bilingual message
bilingual_str error_msg = strprintf(
    _("Operation failed: %s"), reason
);

// Call with both debug and user message
GetNotifications().fatalError(error_msg.original, error_msg);
```

### Pattern 2: Flush Error (Debug Only)

```cpp
// No translation needed - debug only
GetNotifications().flushError(
    "Failed to flush undo file to disk"
);
```

### Pattern 3: Typed Warning

```cpp
// Access KernelNotifications-specific method
auto& notif = static_cast<node::KernelNotifications&>(
    GetNotifications()
);

// Set warning
notif.warningSet(
    kernel::Warning::UNKNOWN_NEW_RULES_ACTIVATED,
    _("Unknown new rules activated")
);

// Later, clear warning
notif.warningUnset(kernel::Warning::UNKNOWN_NEW_RULES_ACTIVATED);
```

---

## Best Practices

### 1. Use Appropriate Error Levels

- **fatalError()** - Only for unrecoverable errors
- **flushError()** - For flush failures that can be ignored
- **warning()** - For conditions user should know about

### 2. Provide Both Message Types

For fatal errors, always provide:
- Debug message (English, technical details)
- User message (translated, user-friendly)

```cpp
// Good
bilingual_str msg = _("Failed to connect");
notifications.fatalError(msg.original, msg);

// Bad - loses translation
notifications.fatalError("Failed to connect", {});
```

### 3. Document Type Casting

When casting to `KernelNotifications`, add a comment:

```cpp
// Note: warningSet is specific to node::KernelNotifications
// In production, GetNotifications() always returns KernelNotifications
auto& notif = static_cast<node::KernelNotifications&>(
    m_chainman.GetNotifications()
);
```

### 4. Use Appropriate Warning Types

```cpp
enum class Warning {
    UNKNOWN_NEW_RULES_ACTIVATED,
    LARGE_WORK_INVALID_CHAIN,
};
```

Choose the correct warning type for your use case.

---

## Migration Guide

### From Old API to New API

#### fatalError()

**Old:**
```cpp
notifications.fatalError(bilingual_str_message);
```

**New:**
```cpp
notifications.fatalError(
    message.original,  // debug message
    message           // user message
);
```

#### flushError()

**Old:**
```cpp
notifications.flushError(_("Flush failed"));
```

**New:**
```cpp
notifications.flushError("Flush failed");  // No translation
```

#### warningSet() / warningUnset()

**Old:**
```cpp
notifications.warningSet(id, message);  // Didn't exist in base
```

**New:**
```cpp
static_cast<node::KernelNotifications&>(notifications)
    .warningSet(id, message);
```

---

## Testing

### Unit Test Example

```cpp
BOOST_AUTO_TEST_CASE(fatal_error_test)
{
    util::SignalInterrupt interrupt;
    std::atomic<int> exit_status{0};
    node::Warnings warnings;
    
    node::KernelNotifications notif(interrupt, exit_status, warnings);
    notif.m_shutdown_on_fatal_error = false;
    
    // Test with both parameters
    notif.fatalError("Debug", Untranslated("User message"));
    
    BOOST_CHECK(true);  // Verify no crash
}
```

---

## Troubleshooting

### Compilation Error: "no matching function"

**Problem:**
```
error: no matching function for call to 'fatalError(bilingual_str)'
```

**Solution:** Pass both debug and user messages:
```cpp
bilingual_str msg = _("Error");
notifications.fatalError(msg.original, msg);
```

### Compilation Error: "has no member named 'warningSet'"

**Problem:**
```
error: 'class kernel::Notifications' has no member named 'warningSet'
```

**Solution:** Cast to `node::KernelNotifications`:
```cpp
auto& n = static_cast<node::KernelNotifications&>(notifications);
n.warningSet(...);
```

---

## Future Considerations

### Potential Improvements

1. **Add warningSet to Base Interface**
   - Pro: No casting needed
   - Con: Breaks abstraction

2. **Capability Pattern**
   - Query for capabilities before calling
   - More flexible but more complex

3. **Separate Warning Interface**
   - Cleaner separation of concerns
   - More interfaces to manage

---

## References

- Base Interface: `src/kernel/notifications_interface.h`
- Implementation: `src/node/kernel_notifications.h`
- Implementation: `src/node/kernel_notifications.cpp`
- Warning Types: `src/kernel/warning.h`
- Usage Examples: `src/validation.cpp`, `src/node/blockstorage.cpp`

---

**Questions?** Refer to the code review documentation or ask on the developer mailing list.

