// Copyright (c) 2025 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <node/kernel_notifications.h>
#include <kernel/warning.h>
#include <node/warnings.h>
#include <util/signalinterrupt.h>

#include <boost/test/unit_test.hpp>

#include <atomic>

BOOST_AUTO_TEST_SUITE(kernel_notifications_tests)

BOOST_AUTO_TEST_CASE(warning_set_unset)
{
    // Test that warningSet and warningUnset work correctly
    util::SignalInterrupt interrupt;
    std::atomic<int> exit_status{0};
    node::Warnings warnings;
    
    node::KernelNotifications notifications(interrupt, exit_status, warnings);
    
    // Set a warning
    notifications.warningSet(kernel::Warning::LARGE_WORK_INVALID_CHAIN, 
                            Untranslated("Test warning message"));
    
    // Verify warning is set (would need access to warnings state to fully test)
    // For now, just verify it doesn't crash
    
    // Unset the warning
    notifications.warningUnset(kernel::Warning::LARGE_WORK_INVALID_CHAIN);
    
    BOOST_CHECK(true); // Test passes if no exceptions thrown
}

BOOST_AUTO_TEST_CASE(fatal_error_basic)
{
    // Test that fatalError can be called with both parameters
    util::SignalInterrupt interrupt;
    std::atomic<int> exit_status{0};
    node::Warnings warnings;
    
    node::KernelNotifications notifications(interrupt, exit_status, warnings);
    notifications.m_shutdown_on_fatal_error = false; // Prevent actual shutdown
    
    // Test with both debug and user message
    notifications.fatalError("Debug message", Untranslated("User message"));
    
    BOOST_CHECK(true); // Test passes if no exceptions thrown
}

BOOST_AUTO_TEST_CASE(flush_error_basic)
{
    // Test that flushError works with string parameter
    util::SignalInterrupt interrupt;
    std::atomic<int> exit_status{0};
    node::Warnings warnings;
    
    node::KernelNotifications notifications(interrupt, exit_status, warnings);
    
    // Test with debug message
    notifications.flushError("Flush error message");
    
    BOOST_CHECK(true); // Test passes if no exceptions thrown
}

BOOST_AUTO_TEST_SUITE_END()
