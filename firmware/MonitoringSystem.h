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

enum class SmokeResult
{
    ModeNotSelected,
    CriticalCreated,
    CriticalAlreadyActive
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

    SmokeResult detectSmokeActivity();
    bool resolveSmokeIncident();
    unsigned int getSmokeIncidentId() const;
    bool markSmokeNotResolved(unsigned int incidentId);

private:
    MonitoringMode mode_ = MonitoringMode::Unselected;

    bool doorIncidentActive_ = false;
    bool doorNotResolved_ = false;
    unsigned int doorIncidentId_ = 0;
    unsigned int nextIncidentId_ = 1;
    bool doorMonitoringEnabled_ = true;
    bool smokeIncidentActive_ = false;
    unsigned int smokeIncidentId_ = 0;
    bool smokeNotResolved_ = false;
};