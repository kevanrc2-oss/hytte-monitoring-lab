#include "MonitoringSystem.h"

void MonitoringSystem::setMode(MonitoringMode mode)
{
    mode_ = mode;
}

MonitoringMode MonitoringSystem::getMode() const
{
    return mode_;
}

DoorResult MonitoringSystem::detectDoorActivity()
{
    if (mode_ == MonitoringMode::Unselected)
    {
        return DoorResult::ModeNotSelected;
    }

    if (mode_ == MonitoringMode::Home)
    {
        return DoorResult::Ignored;
    }

    if (doorIncidentActive_)
    {
        return DoorResult::WarningAlreadyActive;
    }

    doorIncidentId_ = nextIncidentId_++;
    doorIncidentActive_ = true;
    doorNotResolved_ = false;

    return DoorResult::WarningCreated;
}

unsigned int MonitoringSystem::getDoorIncidentId() const
{
    return doorIncidentId_;
}

bool MonitoringSystem::resolveDoorIncident()
{
    if (!doorIncidentActive_)
    {
        return false;
    }

    doorIncidentActive_ = false;
    doorNotResolved_ = false;

    return true;
}

bool MonitoringSystem::markDoorNotResolved(unsigned int incidentId)
{
    if (!doorIncidentActive_ || incidentId != doorIncidentId_)
    {
        return false;
    }

    doorNotResolved_ = true;

    return true;
}