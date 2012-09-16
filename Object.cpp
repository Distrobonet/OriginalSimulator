//
// Filename:        "Object.cpp"
//
// Programmer:      Ross Mead
// Last modified:   22Dec2010
//
// Description:     This class implements a 2-dimensional object.
//

// preprocessor directives
#include "Environment.h"
#include "Object.h"



// <protected static data members>
GLint Object::nObjects = 0;   // initializes the number of objects to 0



// <constructors>

//
// Object(dx, dy, dz, colorIndex)
// Last modified: 22Dec2010
//
// Default constructor that initializes
// this object to the parameterized values.
//
// Returns:     <none>
// Parameters:
//      dx          in      the initial x-coordinate of the object (default 0)
//      dy          in      the initial y-coordinate of the object (default 0)
//      dz          in      the initial z-coordinate of the object (default 0)
//      colorIndex  in      the initial array index of the color of the object
//
Object::Object(const GLfloat dx, const GLfloat dy, const GLfloat dz,
               const Color   colorIndex)
{
    init(dx, dy, dz, colorIndex);
    ID = ++nObjects;
}   // Object(const GLfloat, const GLfloat, const GLfloat, const Color)



//
// Object(obj)
// Last modified: 22Dec2010
//
// Copy constructor that copies the contents of
// the parameterized object into this object.
//
// Returns:     <none>
// Parameters:
//      obj     in/out      the object being copied
//
Object::Object(const Object &obj)
{
    init(obj.x, obj.y, obj.z, DEFAULT_OBJECT_COLOR);
    setColor(obj.color);
    for (GLint i = 0; i < 3; ++i)
    {
        translate[i] = obj.translate[i];
        rotate[i]    = obj.rotate[i];
        scale[i]     = obj.scale[i];
    }
	showFilled  = obj.showFilled;
    showPos     = obj.showPos;
    ID          = obj.ID;
    env         = obj.env;
}   // Object(const Object &)



// <destructors>

//
// ~Object()
// Last modified: 22Dec2010
//
// Destructor that clears this object.
//
// Returns:     <none>
// Parameters:  <none>
//
Object::~Object()
{
}   // ~Object()



// <virtual public mutator functions>

//
// bool setRadius(r)
// Last modified: 22Dec2010
//
// Attempts to set the radius to the parameterized radius,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      r       in      the radius to be set to
//
bool Object::setRadius(const GLfloat r)
{
    if (!Circle::setRadius(r)) return false;
    return behavior.setMaxSpeed(maxSpeed());
}   // setRadius(const GLfloat)



//
// bool setEnvironment(e)
// Last modified: 22Dec2010
//
// Attempts to set the environment to the parameterized environment,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      e       in      the envirnment to be set to
//
bool Object::setEnvironment(Environment *e)
{
    env = e;
    return true;
}   // setEnvironment(Environment *)



// <virtual public accessor functions>

//
// Environment* getEnvironment() const
// Last modified: 03Sep2006
//
// Returns the environment of this object.
//
// Returns:     the environment of this object
// Parameters:  <none>
//
Environment* Object::getEnvironment() const
{
    return env;
}   // getEnvironment() const



// <public accessor functions>

//
// GLint getID() const
// Last modified: 22Dec2010
//
// Returns the ID of this object.
//
// Returns:     the ID of this object
// Parameters:  <none>
//
GLint Object::getID() const
{
    return ID;
}   // getID()



// <virtual public utility functions>

//
// void draw()
// Last modified: 22Dec2010
//
// Renders the object as a circle with a vector heading.
//
// Returns:     <none>
// Parameters:  <none>
//
void Object::draw()
{
    if ((color[GLUT_RED]   == COLOR[INVISIBLE][GLUT_RED])   &&
        (color[GLUT_GREEN] == COLOR[INVISIBLE][GLUT_GREEN]) &&
        (color[GLUT_BLUE]  == COLOR[INVISIBLE][GLUT_BLUE])) return;

    Circle::draw();   // draw a circle representing the object
}   // draw()



// <virtual protected utility functions>

//
// bool init(dx, dy, dz, theta, colorIndex)
// Last modified: 06Nov2009
//
// Initializes the object to the parameterized values,
// returning true if successful, false otherwise.
//
// Returns:     true if successful, false otherwise
// Parameters:
//      dx          in      the initial x-coordinate of the object (default 0)
//      dy          in      the initial y-coordinate of the object (default 0)
//      dz          in      the initial z-coordinate of the object (default 0)
//      colorIndex  in      the initial array index of the color of the object
//
bool Object::init(const GLfloat dx,    const GLfloat dy, const GLfloat dz,
                 const GLfloat theta, const Color   colorIndex)
{
    Circle::init(dx, dy, dz, DEFAULT_OBJECT_RADIUS, colorIndex);
    setHeading(theta);
    behavior         = DEFAULT_OBJECT_BEHAVIOR;
    behavior.setMaxSpeed(maxSpeed());
    showFilled       = DEFAULT_OBJECT_SHOW_FILLED;
    setEnvironment(NULL);
    return true;
}   // init(const GLfloat..<4>, const Color)

