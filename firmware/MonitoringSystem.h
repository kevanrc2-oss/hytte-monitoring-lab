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
    unsigned int getDoorIncidentId() const;

private:
    MonitoringMode mode_ = MonitoringMode::Unselected;

    bool doorIncidentActive_ = false;
    unsigned int doorIncidentId_ = 0;
    unsigned int nextIncidentId_ = 1;
};