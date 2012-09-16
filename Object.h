//
// Filename:        "Object.h"
//
// Programmer:      Ross Mead
// Last modified:   22Dec2010
//
// Description:     This class describes a 2-dimensional object.
//

// preprocessor directives
#ifndef OBJECT_H
#define OBJECT_H
#include "Circle.h"
using namespace std;



// global constants
static const Color    DEFAULT_OBJECT_COLOR        = DEFAULT_CIRCLE_COLOR;
static const GLfloat  DEFAULT_OBJECT_RADIUS       = 0.03f;
static const bool     DEFAULT_OBJECT_SHOW_FILLED  = false;



// forward declaration of an object environment
class Environment;



// describes a 2-dimensional object
class Object: public Circle
{

    public:

        // <constructors>
        Object(const GLfloat dx         = 0.0f,
               const GLfloat dy         = 0.0f,
               const GLfloat dz         = 0.0f,
               const Color   colorIndex = DEFAULT_OBJECT_COLOR);
        Object(const Object &obj);

        // <destructors>
        virtual ~Object();

        // <virtual public mutator functions>
        virtual bool setRadius(const GLfloat r = DEFAULT_OBJECT_RADIUS);
        virtual bool setEnvironment(Environment *e);

        // <virtual public accessor functions>
        virtual Environment* getEnvironment() const;

        // <public accessor functions>
        GLint getID() const;

        // <virtual public utility functions>
        virtual void draw();

    protected:

        // <protected data members>
        GLint         ID;     // identification number of object
        Environment  *env;    // the environment of the object

        // <protected static data members>
        static GLint  nObjects;    // number of total objects

        // <virtual protected utility functions>
        virtual bool init(const GLfloat dx         = 0.0f,
                          const GLfloat dy         = 0.0f,
                          const GLfloat dz         = 0.0f,
                          const GLfloat theta      = 0.0f,
                          const Color   colorIndex = DEFAULT_OBJECT_COLOR);
};  // Object

#endif
