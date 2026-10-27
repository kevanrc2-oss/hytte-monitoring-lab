#pragma once

enum class MonitoringMode
{
    Unselected,
    Home,
    Away
};

enum class DoorResult
{
    ModeNotSelected,
    Ignored,
    WarningCreated,
    WarningAlreadyActive
};

class MonitoringSystem
{
public:
    void setMode(MonitoringMode mode);
    MonitoringMode getMode() const;

    DoorResult detectDoorActivity();
    bool resolveDoorIncident();
    bool markDoorNotResolved(unsigned int incidentId);

    unsigned int getDoorIncidentId() const;
    void setDoorMonitoringEnabled(bool enabled);
    bool isDoorMonitoringEnabled() const;

private:
    MonitoringMode mode_ = MonitoringMode::Unselected;

    bool doorIncidentActive_ = false;
    bool doorNotResolved_ = false;
    unsigned int doorIncidentId_ = 0;
    unsigned int nextIncidentId_ = 1;
    bool doorMonitoringEnabled_ = true;
};