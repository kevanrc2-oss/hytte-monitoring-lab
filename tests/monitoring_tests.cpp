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

    std::cout << "All monitoring tests passed.\n";
}