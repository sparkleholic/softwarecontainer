/*
 *   Copyright (C) 2014 Pelagicore AB
 *   All rights reserved.
 */
#ifndef ABSTRACTCONTROLLER_H
#define ABSTRACTCONTROLLER_H

/*! AbstractController is an abstract interface to the Controller class
 *
 *  It's used by the IPC implementation to call methods on an implementation
 *  of this interface.
 */
class AbstractController {

public:
    virtual ~AbstractController() {};

    /*! Starts the application inside the container
     *
     * \return Pid of the application process
     */
    virtual int runApp() = 0;

    /*! Stops the application running inside the container, and also stops
     * Controller
     */
    virtual void killApp() = 0;
};

#endif //ABSTRACTCONTROLLER_H