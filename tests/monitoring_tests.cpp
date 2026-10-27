#include "MonitoringSystem.h"

#include <cstdlib>
#include <iostream>

void check(bool condition, const char* description)
{
    if (!condition)
    {
        std::cerr << "FAIL: " << description << '\n';
        std::exit(EXIT_FAILURE);
    }

    std::cout << "PASS: " << description << '\n';
}

int main()
{
    MonitoringSystem monitoring;

    check(
        monitoring.getMode() == MonitoringMode::Unselected,
        "System starts without a selected mode"
    );

    check(
        monitoring.detectDoorActivity() == DoorResult::ModeNotSelected,
        "Door activity is rejected before mode selection"
    );

    monitoring.setMode(MonitoringMode::Home);

    check(
        monitoring.detectDoorActivity() == DoorResult::Ignored,
        "Door activity in HOME is ignored"
    );

    check(
        monitoring.getDoorIncidentId() == 0,
        "Ignored activity does not allocate an Incident ID"
    );

    monitoring.setMode(MonitoringMode::Away);

    check(
        monitoring.detectDoorActivity() == DoorResult::WarningCreated,
        "Door activity in AWAY creates a Warning"
    );

    const unsigned int firstId = monitoring.getDoorIncidentId();

    check(
        firstId > 0,
        "Created Incident has a valid ID"
    );

    check(
        monitoring.detectDoorActivity() == DoorResult::WarningAlreadyActive,
        "Repeated activity retains the active Warning"
    );

    check(
        monitoring.getDoorIncidentId() == firstId,
        "Repeated activity preserves the Incident ID"
    );

    check(
        !monitoring.markDoorNotResolved(firstId + 1),
        "Not Resolved rejects an incorrect Incident ID"
    );

    check(
        monitoring.markDoorNotResolved(firstId),
        "Not Resolved accepts the active Incident"
    );

    check(
        monitoring.detectDoorActivity() == DoorResult::WarningAlreadyActive,
        "Incident remains Active after Not Resolved"
    );

    check(
        monitoring.getDoorIncidentId() == firstId,
        "Not Resolved preserves the Incident ID"
    );

    check(
        monitoring.resolveDoorIncident(),
        "Active Incident can be resolved"
    );

    check(
        monitoring.getDoorIncidentId() == firstId,
        "Closing preserves the Incident ID"
    );

    check(
        !monitoring.markDoorNotResolved(firstId),
        "Closed Incident rejects Not Resolved"
    );

    check(
        !monitoring.resolveDoorIncident(),
        "An Incident cannot be closed twice"
    );

    check(
        monitoring.detectDoorActivity() == DoorResult::WarningCreated,
        "Activity after closure creates a new Incident"
    );

    check(
        monitoring.getDoorIncidentId() > firstId,
        "New Incident receives a new ID"
    );

        MonitoringSystem configured;
    configured.setMode(MonitoringMode::Away);

    check(
        configured.isDoorMonitoringEnabled(),
        "Door monitoring is enabled by default"
    );

    configured.setDoorMonitoringEnabled(false);

    check(
        !configured.isDoorMonitoringEnabled(),
        "Door monitoring can be disabled"
    );

    check(
        configured.detectDoorActivity() == DoorResult::Ignored,
        "Disabled Door monitoring ignores activity in AWAY"
    );

    check(
        configured.getDoorIncidentId() == 0,
        "Disabled monitoring does not allocate an Incident ID"
    );

    configured.setDoorMonitoringEnabled(true);

    check(
        configured.detectDoorActivity() == DoorResult::WarningCreated,
        "Re-enabled Door monitoring creates a Warning in AWAY"
    );

    configured.setDoorMonitoringEnabled(false);

    check(
        configured.resolveDoorIncident(),
        "Disabling monitoring preserves an existing active Incident"
    );

        MonitoringSystem smokeHome;

    check(
        smokeHome.detectSmokeActivity() == SmokeResult::ModeNotSelected,
        "Smoke simulation waits for startup mode selection"
    );

    smokeHome.setMode(MonitoringMode::Home);
    smokeHome.setDoorMonitoringEnabled(false);

    check(
        smokeHome.detectSmokeActivity() == SmokeResult::CriticalCreated,
        "Smoke creates a Critical Incident in HOME with Door monitoring off"
    );

    const unsigned int smokeHomeId = smokeHome.getSmokeIncidentId();

    check(
        smokeHome.detectSmokeActivity() ==
            SmokeResult::CriticalAlreadyActive,
        "Repeated smoke activity retains the active Critical Incident"
    );

    check(
        smokeHome.getSmokeIncidentId() == smokeHomeId,
        "Repeated smoke activity preserves the Incident ID"
    );

        check(
        !smokeHome.markSmokeNotResolved(smokeHomeId + 1),
        "Smoke Not Resolved rejects an incorrect Incident ID"
    );

    check(
        smokeHome.markSmokeNotResolved(smokeHomeId),
        "Smoke Not Resolved accepts the active Incident"
    );

    check(
        smokeHome.detectSmokeActivity() ==
            SmokeResult::CriticalAlreadyActive,
        "Smoke Incident remains Active after Not Resolved"
    );

    check(
        smokeHome.getSmokeIncidentId() == smokeHomeId,
        "Smoke Not Resolved preserves the Incident ID"
    );

    check(
        smokeHome.resolveSmokeIncident(),
        "An active Smoke Incident can be resolved"
    );

        check(
        !smokeHome.markSmokeNotResolved(smokeHomeId),
        "Closed Smoke Incident rejects Not Resolved"
    );

    check(
        smokeHome.getSmokeIncidentId() == smokeHomeId,
        "Closing a Smoke Incident preserves its ID"
    );

    check(
        !smokeHome.resolveSmokeIncident(),
        "A Smoke Incident cannot be closed twice"
    );

    check(
        smokeHome.detectSmokeActivity() == SmokeResult::CriticalCreated,
        "Smoke activity after closure creates a new Critical Incident"
    );

    check(
        smokeHome.getSmokeIncidentId() > smokeHomeId,
        "A new Smoke Incident receives a new ID"
    );

    MonitoringSystem smokeAway;
    smokeAway.setMode(MonitoringMode::Away);
    smokeAway.setDoorMonitoringEnabled(false);

    check(
        smokeAway.detectSmokeActivity() == SmokeResult::CriticalCreated,
        "Smoke creates a Critical Incident in AWAY with Door monitoring off"
    );

    smokeAway.setDoorMonitoringEnabled(true);

    check(
        smokeAway.detectDoorActivity() == DoorResult::WarningCreated,
        "Door and Smoke Incidents can be active simultaneously"
    );

    check(
        smokeAway.getDoorIncidentId() != smokeAway.getSmokeIncidentId(),
        "Door and Smoke Incidents have different IDs"
    );

    check(
        smokeAway.resolveDoorIncident(),
        "The simultaneous Door Incident can be closed"
    );

    check(
        smokeAway.detectSmokeActivity() ==
            SmokeResult::CriticalAlreadyActive,
        "Closing the Door Incident leaves the Smoke Incident active"
    );

    std::cout << "All monitoring tests passed.\n";
}