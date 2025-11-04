# Feature Enhancement Proposals

**Date:** November 4, 2025  
**Priority:** Future Development  
**Purpose:** Option 8 - Feature Development

---

## Overview

This document proposes feature enhancements for the Bitcoin Core notification system based on the current architecture and identified opportunities.

## Feature 1: Enhanced Warning System with Categories

### Current State
- Binary warning system (set/unset)
- Two warning types
- No severity levels
- No categorization

### Proposed Enhancement

```cpp
namespace kernel {
    enum class WarningCategory {
        NETWORK,        // Network-related warnings
        CONSENSUS,      // Consensus rule warnings
        STORAGE,        // Disk/storage warnings
        PERFORMANCE,    // Performance degradation
        SECURITY        // Security concerns
    };
    
    enum class WarningSeverity {
        INFO,           // Informational
        WARNING,        // Warning
        CRITICAL        // Critical issue
    };
    
    struct WarningDetails {
        Warning id;
        WarningCategory category;
        WarningSeverity severity;
        bilingual_str message;
        std::chrono::system_clock::time_point timestamp;
        std::optional<std::string> remedy;  // Suggested fix
    };
    
    class EnhancedNotifications : public Notifications {
    public:
        virtual void warningSet(const WarningDetails& details) = 0;
        virtual std::vector<WarningDetails> getActiveWarnings() const = 0;
        virtual std::vector<WarningDetails> getWarningHistory(
            std::chrono::hours duration) const = 0;
    };
}
```

### Use Cases
1. Filter warnings by category
2. Prioritize by severity
3. Track warning history
4. Suggest remediation steps

### Implementation Effort
**Medium** (1-2 weeks)

---

## Feature 2: Notification Filtering and Routing

### Current State
- All notifications go to single handler
- No filtering capability
- No routing logic

### Proposed Enhancement

```cpp
namespace kernel {
    class NotificationFilter {
    public:
        virtual bool shouldNotify(const WarningDetails& warning) const = 0;
        virtual bool shouldNotify(const std::string& error_type) const = 0;
    };
    
    class NotificationRouter {
    private:
        std::vector<std::pair<NotificationFilter*, Notifications*>> routes;
        
    public:
        void addRoute(NotificationFilter* filter, Notifications* handler) {
            routes.emplace_back(filter, handler);
        }
        
        void notifyWarning(const WarningDetails& warning) {
            for (auto& [filter, handler] : routes) {
                if (filter->shouldNotify(warning)) {
                    handler->warning(warning.message);
                }
            }
        }
    };
}

// Example filters
class SeverityFilter : public NotificationFilter {
    WarningSeverity min_severity;
public:
    SeverityFilter(WarningSeverity min) : min_severity(min) {}
    
    bool shouldNotify(const WarningDetails& warning) const override {
        return warning.severity >= min_severity;
    }
};

class CategoryFilter : public NotificationFilter {
    std::set<WarningCategory> categories;
public:
    bool shouldNotify(const WarningDetails& warning) const override {
        return categories.count(warning.category) > 0;
    }
};
```

### Use Cases
1. Route critical warnings to operators
2. Filter noise for developers
3. Category-specific handling
4. Custom notification logic

### Implementation Effort
**Medium** (2-3 weeks)

---

## Feature 3: Pluggable Notification Handlers

### Current State
- Single hardcoded implementation
- No plugin support
- Static linking

### Proposed Enhancement

```cpp
namespace kernel {
    class NotificationHandler {
    public:
        virtual ~NotificationHandler() = default;
        
        virtual std::string name() const = 0;
        virtual void initialize(const Config& config) = 0;
        virtual void handleWarning(const WarningDetails& warning) = 0;
        virtual void handleError(const std::string& error) = 0;
        virtual void shutdown() = 0;
    };
    
    class NotificationManager {
    private:
        std::vector<std::unique_ptr<NotificationHandler>> handlers;
        
    public:
        void registerHandler(std::unique_ptr<NotificationHandler> handler) {
            handler->initialize(config);
            handlers.push_back(std::move(handler));
        }
        
        void notifyAll(const WarningDetails& warning) {
            for (auto& handler : handlers) {
                handler->handleWarning(warning);
            }
        }
    };
}

// Example handlers
class LogFileHandler : public NotificationHandler {
    std::ofstream logfile;
public:
    void handleWarning(const WarningDetails& warning) override {
        logfile << warning.timestamp << " " 
                << warning.category << " "
                << warning.message.original << std::endl;
    }
};

class EmailHandler : public NotificationHandler {
    // Send emails for critical warnings
};

class MetricsHandler : public NotificationHandler {
    // Send to monitoring system (Prometheus, etc.)
};
```

### Use Cases
1. Multiple notification destinations
2. Custom alerting systems
3. Monitoring integration
4. Audit logging

### Implementation Effort
**High** (3-4 weeks)

---

## Feature 4: Notification Persistence

### Current State
- Warnings not persisted
- Lost on restart
- No history

### Proposed Enhancement

```cpp
namespace kernel {
    class NotificationStore {
    public:
        virtual void save(const WarningDetails& warning) = 0;
        virtual std::vector<WarningDetails> loadActive() const = 0;
        virtual std::vector<WarningDetails> loadHistory(
            std::chrono::system_clock::time_point since) const = 0;
        virtual void clear(Warning id) = 0;
        virtual void clearAll() = 0;
    };
    
    class LevelDBNotificationStore : public NotificationStore {
        // Persist to LevelDB
    };
    
    class SQLiteNotificationStore : public NotificationStore {
        // Persist to SQLite
    };
}
```

### Use Cases
1. Warning persistence across restarts
2. Historical analysis
3. Compliance/audit trails
4. Debugging

### Implementation Effort
**Medium** (2-3 weeks)

---

## Feature 5: Structured Logging Integration

### Current State
- Plain text messages
- No structure
- Hard to parse

### Proposed Enhancement

```cpp
namespace kernel {
    struct LogEntry {
        std::string level;  // INFO, WARNING, ERROR
        std::string component;
        std::string message;
        std::map<std::string, std::string> fields;
        std::chrono::system_clock::time_point timestamp;
        
        // Serialize to JSON
        std::string toJSON() const;
    };
    
    class StructuredLogger {
    public:
        void log(const LogEntry& entry) {
            // Output JSON or other structured format
            std::cout << entry.toJSON() << std::endl;
        }
    };
    
    // Integration with notifications
    class StructuredNotifications : public Notifications {
        StructuredLogger logger;
        
    public:
        void warning(const bilingual_str& message) override {
            LogEntry entry;
            entry.level = "WARNING";
            entry.component = "notifications";
            entry.message = message.original;
            entry.timestamp = std::chrono::system_clock::now();
            logger.log(entry);
        }
    };
}
```

### Use Cases
1. Log aggregation (ELK, Splunk)
2. Automated parsing
3. Machine-readable logs
4. Better monitoring

### Implementation Effort
**Low** (1 week)

---

## Feature 6: Notification Rate Limiting

### Current State
- No rate limiting
- Possible notification storms
- Performance impact

### Proposed Enhancement

```cpp
namespace kernel {
    class RateLimiter {
        std::map<Warning, std::deque<std::chrono::system_clock::time_point>> history;
        std::chrono::seconds window{60};
        size_t max_per_window{10};
        
    public:
        bool shouldAllow(Warning id) {
            auto now = std::chrono::system_clock::now();
            auto& times = history[id];
            
            // Remove old entries
            while (!times.empty() && 
                   (now - times.front()) > window) {
                times.pop_front();
            }
            
            if (times.size() >= max_per_window) {
                return false;  // Rate limited
            }
            
            times.push_back(now);
            return true;
        }
    };
    
    class RateLimitedNotifications : public Notifications {
        RateLimiter limiter;
        
    public:
        void warningSet(Warning id, const bilingual_str& message) override {
            if (limiter.shouldAllow(id)) {
                // Proceed with notification
            } else {
                // Suppress (maybe log suppression)
            }
        }
    };
}
```

### Use Cases
1. Prevent notification storms
2. Protect performance
3. Reduce noise
4. Configurable limits

### Implementation Effort
**Low** (1 week)

---

## Feature 7: User-Configurable Notifications

### Current State
- Hardcoded behavior
- No user control
- Fixed verbosity

### Proposed Enhancement

```cpp
// bitcoin.conf
notification.verbosity=high
notification.categories=network,consensus
notification.min_severity=warning
notification.handlers=log,email,metrics
notification.rate_limit=10/minute

// Code
struct NotificationConfig {
    enum class Verbosity { LOW, MEDIUM, HIGH };
    
    Verbosity verbosity;
    std::set<WarningCategory> enabled_categories;
    WarningSeverity min_severity;
    std::vector<std::string> handlers;
    RateLimitConfig rate_limit;
    
    static NotificationConfig fromArgs(const ArgsManager& args);
};
```

### Use Cases
1. User preferences
2. Operator requirements
3. Debug vs production
4. Compliance needs

### Implementation Effort
**Medium** (1-2 weeks)

---

## Feature 8: Notification Dashboard/API

### Current State
- No external access
- Command-line only
- No monitoring

### Proposed Enhancement

```cpp
// RPC endpoints
{
    "method": "getwarnings",
    "result": [
        {
            "id": "LARGE_WORK_INVALID_CHAIN",
            "category": "consensus",
            "severity": "critical",
            "message": "Invalid chain detected",
            "timestamp": "2025-11-04T18:00:00Z",
            "remedy": "Check peer connections"
        }
    ]
}

{
    "method": "getwarninghistory",
    "params": {"hours": 24},
    "result": [...]
}

{
    "method": "clearwarning",
    "params": {"id": "LARGE_WORK_INVALID_CHAIN"}
}
```

### Use Cases
1. External monitoring
2. Dashboard integration
3. Automated alerting
4. API access

### Implementation Effort
**Medium** (2 weeks)

---

## Implementation Priority

### High Priority (Next Quarter)
1. **Feature 1:** Enhanced Warning System ⭐
2. **Feature 6:** Rate Limiting ⭐
3. **Feature 7:** User Configuration ⭐

### Medium Priority (Next 6 Months)
4. **Feature 2:** Filtering and Routing
5. **Feature 5:** Structured Logging
6. **Feature 8:** Dashboard/API

### Low Priority (Future)
7. **Feature 3:** Pluggable Handlers
8. **Feature 4:** Persistence

---

## Integration Plan

### Phase 1: Foundation
1. Enhance warning data structure
2. Add rate limiting
3. Add configuration support

### Phase 2: Advanced
1. Implement filtering/routing
2. Add structured logging
3. Create RPC endpoints

### Phase 3: Extensibility
1. Plugin support
2. Persistence layer
3. External integrations

---

## Backward Compatibility

All features designed for backward compatibility:
- New features opt-in
- Existing code continues to work
- Graceful degradation
- Configuration-driven

---

## Testing Strategy

### Unit Tests
- Each feature independently testable
- Mock implementations
- Edge cases covered

### Integration Tests
- Feature interactions
- End-to-end scenarios
- Performance impact

### User Acceptance
- Beta testing
- Feedback collection
- Iterative improvement

---

## Conclusions

### Recommended Next Steps

1. **Immediate:** Implement Feature 6 (Rate Limiting)
   - Low effort, high value
   - No breaking changes
   - Clear benefit

2. **Short-term:** Implement Feature 1 (Enhanced Warnings)
   - Foundation for other features
   - Significant value add
   - Manageable scope

3. **Medium-term:** Gather feedback and prioritize
   - User needs assessment
   - Operator requirements
   - Strategic alignment

### Success Criteria

- ✅ Improved operator experience
- ✅ Better debugging capabilities
- ✅ Enhanced monitoring integration
- ✅ No performance regression
- ✅ Backward compatible

---

**Status:** Option 8 Complete ✅  
**Next Steps:** Discuss priorities with stakeholders, create detailed specs

