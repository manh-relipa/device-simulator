#ifndef ISETTING_H
#define ISETTING_H
#include <QSettings>
#include <memory>

#define SETTING_FILE "settings.ini"
class ISetting
{
protected:
    std::shared_ptr<QSettings> m_settings;
public:
    virtual void loadSetting() = 0;
};

#endif // ISETTING_H
