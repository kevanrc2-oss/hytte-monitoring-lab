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

    return DoorResult::WarningCreated;
}

unsigned int MonitoringSystem::getDoorIncidentId() const
{
    return doorIncidentId_;
}