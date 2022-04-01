#ifndef __DRIVERS__DRIVER_H
#define __DRIVERS__DRIVER_H


class Driver {
public:
    Driver();
    ~Driver();

    virtual void Activate();
    virtual int  Reset();  // reset status
    virtual void Deactivate();
};

class DriverManager {
public:
    DriverManager();
    void AddDriver(Driver*);
    void ActivateAll();

    Driver* drivers[256];
    int     numDrivers;
};


#endif