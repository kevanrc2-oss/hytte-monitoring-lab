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

    if (mode_ == MonitoringMode::Home || !doorMonitoringEnabled_)
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

void MonitoringSystem::setDoorMonitoringEnabled(bool enabled)
{
    doorMonitoringEnabled_ = enabled;
}

bool MonitoringSystem::isDoorMonitoringEnabled() const
{
    return doorMonitoringEnabled_;
}

SmokeResult MonitoringSystem::detectSmokeActivity()
{
    if (mode_ == MonitoringMode::Unselected)
    {
        return SmokeResult::ModeNotSelected;
    }

    if (smokeIncidentActive_)
    {
        return SmokeResult::CriticalAlreadyActive;
    }

    smokeIncidentId_ = nextIncidentId_++;
    smokeIncidentActive_ = true;
    smokeNotResolved_ = false;

    return SmokeResult::CriticalCreated;
}

bool MonitoringSystem::resolveSmokeIncident()
{
    if (!smokeIncidentActive_)
    {
        return false;
    }

    smokeIncidentActive_ = false;
    smokeNotResolved_ = false;

    return true;
}

unsigned int MonitoringSystem::getSmokeIncidentId() const
{
    return smokeIncidentId_;
}

bool MonitoringSystem::markSmokeNotResolved(unsigned int incidentId)
{
    if (!smokeIncidentActive_ || incidentId != smokeIncidentId_)
    {
        return false;
    }

    smokeNotResolved_ = true;

    return true;
}