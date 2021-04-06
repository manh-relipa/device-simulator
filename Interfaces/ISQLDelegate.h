#ifndef ISQLDELEGATE_H
#define ISQLDELEGATE_H
#include <QObject>
#include <QMetaType>
#include <memory>

    class ISQLDelegate {
    public:
        bool synchronized {false};

        virtual void insert() const = 0;
        virtual void update()  = 0;
        virtual void remove()  = 0;
        virtual void loadFromDB() = 0;
    };


    Q_DECLARE_METATYPE(std::shared_ptr<ISQLDelegate>);

    class ISQLObjectMaster: public QObject {
        Q_OBJECT
    signals:
        void insertObject(const std::shared_ptr<ISQLDelegate>);
        void updateObject(const std::shared_ptr<ISQLDelegate>);
        void removeObject(const std::shared_ptr<ISQLDelegate>);
        void loadObject(std::shared_ptr<ISQLDelegate>);
    };




#endif // ISQLDELEGATE_H
