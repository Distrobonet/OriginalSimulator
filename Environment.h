//
// Filename:        "Environment.h"
//
// Programmer:      Ross Mead
// Last modified:   22Dec2010
//
// Description:     This class describes a robot cell environment.
//

// preprocessor directives
#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H
#include <queue>
#include <time.h>
#include <vector>
#include "Cell.h"
using namespace std;



//
#define VERBOSE (0)



// global constants
static const Color DEFAULT_ENV_COLOR = BLACK;



// type redefinition
typedef Circle Object;



// describes a robot cell environment
class Environment
{
    public:

        // <public data members>
        GLfloat color[3];
        bool    startFormation;
        GLint   formationID;
        Vector  centroid;
        GLfloat radius;
        Circle  circle;
        Vector distance;

        // <constructors>
        Environment(const GLint     n          = 0,
                    const Formation f          = Formation(),
                    const Color     colorIndex = DEFAULT_ENV_COLOR);
        Environment(const Environment &e);

        // <destructors>
        virtual ~Environment();

        // <virtual public mutator functions>
        virtual bool setColor(const GLfloat r,
                              const GLfloat g,
                              const GLfloat b);
        virtual bool setColor(const GLfloat clr[3]);
        virtual bool setColor(const Color colorIndex = DEFAULT_VECTOR_COLOR);

        // <public mutator functions>
        bool addCell(Cell *c = NULL);
        bool removeCell();
        bool removeCell(Cell *c);
        bool addRobot(GLfloat x     = 0.0f, GLfloat y = 0.0f, GLfloat z = 0.0f,
                      GLfloat theta = 0.0f);
        bool removeRobot();
        bool removeRobot(Robot *r);
		bool addObject(GLfloat dx = 0.0f,
		               GLfloat dy = 0.0f,
		               GLfloat dz = 0.0f);
		bool addObject(Object *o = NULL);
		bool removeObject();
        bool removeObject(Object *o);

        // <public accessor functions>
        Cell*               getCell(GLint pos) const;
        Robot*              getRobot(GLint id);
		Object*             getObject(GLint index) const;
        vector<Cell *>      getCells();    // BAD!!!
        vector<Robot *>     getRobots();   // BAD!!!
		vector<Object *>    getObjects();  // BAD!!!
        GLint               getNCells() const;
        GLint               getNFreeRobots() const;
		GLint               getNObjects() const;


        // <virtual public utility functions>
        virtual void draw();
        virtual bool step();
        virtual void clear();

        // <public utility functions>
        Vector  getRelationship(const GLint toID, const GLint fromID);
        GLfloat getDistanceTo(const GLint id)   const;
        GLfloat getAngleTo(const GLint id)      const;
        //brntbeer added for Prop_ops
        void    getCentroid( Vector v);
        void    getRadius(GLfloat rad);
        void    getDistance(Vector dist);
		//removed const from parameters
        bool    sendMsg(const Message &msg = NULL,
                        const GLint    toID   = ID_BROADCAST,
                        const GLint    fromID = ID_OPERATOR,
                        const GLint    type   = HEARTBEAT);

		//removed const
        bool    sendPacket(const Packet &p = Packet());
        vector<Cell *>  getCellVector();
        vector<Robot *> getRobotVector();

        // <public sensor functions>
		vector<Vector> getObjectRelationships(
		   const GLint   fromID,
		   const GLfloat maxDist = SENSOR_RANGE);

		//removed const
        bool    forwardPacket(const Packet &p);
        bool    forwardPackets();

        // <public utility cell functions>
        bool    showLine(const bool show);
        bool    showHead(const bool show);
        bool    showPos(const bool show);
        bool    showHeading(const bool show);

        // <public utility auctioning functions>
        //bool    auctionPosition(Cell* a);
        Robot*  getNearestRobot(Cell *c);
        Robot*  getNearestRobot(GLfloat x, GLfloat y);
        GLfloat distanceToRobot(Cell *c,Robot *r);
        GLfloat distanceToRobot(GLfloat x,GLfloat y,Robot *r);
        void    formUp();
        void    formFromClick(GLfloat x,GLfloat y);
        bool    changeFormation(Formation &f);
        bool    changeFormationSeed(Formation &f, GLint id);
        void    settleAuction(Cell* c,GLint rID);
        //brntbeer added for prop_ops
        friend bool changeFormationSim(const GLint index, 
            const Vector gradient);

    protected:

        // <protected data members>
        vector<Cell *>  cells;
        vector<Robot *> robots;
		vector<Object *> objects;
        queue<Packet>       msgQueue;
        Cell               *newestCell;
        Formation           formation;
        GLint               nRobots;
        Color               defaultColor;

        // <virtual protected utility functions>
        virtual bool init(const GLint     n          = 0,
                          const Formation f          = Formation(),
                          const Color     colorIndex = DEFAULT_ENV_COLOR);
        //virtual bool initCells(const GLint     n = 0,
        //                       const Formation f = Formation());
        //virtual bool initNbrs(const GLint nNbrs = 0);
		virtual bool initRobots();
};  // Environment

#endif
