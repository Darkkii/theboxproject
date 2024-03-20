#ifndef SETTINGSMESSAGE_H
#define SETTINGSMESSAGE_H

class SettingsMessage
{
  private:
    bool mMode;
    int mSetpoint;

  public:
    SettingsMessage(const bool mode, const int setpoint);
    bool getAuto();
    int getSetpoint();
};

#endif /* SETTINGSMESSAGE_H */
