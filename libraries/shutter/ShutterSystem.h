// ShutterSystem.h

#ifndef ShutterSystem_h
#define ShutterSystem_h

#include <stdlib.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#include <wiring.h>
#endif

#define SHUTTERSYSTEM_MAX_SHUTTERS 10

class shutter;

/////////////////////////////////////////////////////////////////////
/// \class ShutterSystem ShutterSystem.h <ShutterSystem.h>
/// \brief Operate multiple shutters in a co-ordinated fashion
///
/// This class can manage multiple shutters (up to SHUTTERSYSTEM_MAX_SHUTTERS = 10), 
/// and cause them all to move
/// to selected positions at such a (constant) speed that they all arrive at their
/// target position at the same time. This can be used to support devices with multiple shutters
/// on say multiple axes to cause linear diagonal motion. Suitable for use with X-Y plotters, flatbeds,
/// 3D printers etc
/// to get linear straight line movement between arbitrary 2d (or 3d or ...) positions.
///
/// Caution: only constant speed shutter motion is supported: acceleration and deceleration is not supported
/// All the shutters managed by ShutterSystem will step at a constant speed to their
/// target (albeit perhaps different speeds for each shutter).
class ShutterSystem
{
public:
    /// Constructor
    ShutterSystem();

    /// Add a shutter to the set of managed shutters
    /// There is an upper limit of SHUTTERSYSTEM_MAX_SHUTTERS = 10 to the number of shutters that can be managed
    /// \param[in] shutter Reference to a shutter to add to the managed list
    /// \return true if successful. false if the number of managed shutters would exceed SHUTTERSYSTEM_MAX_SHUTTERS
    boolean addShutter(shutter& shutter);

    /// Set the target positions of all managed shutters 
    /// according to a coordinate array.
    /// New speeds will be computed for each shutter so they will all arrive at their 
    /// respective targets at very close to the same time.
    /// \param[in] absolute An array of desired absolute shutter positions. absolute[0] will be used to set
    /// the absolute position of the first shutter added by addShutter() etc. The array must be at least as long as 
    /// the number of shutters that have been added by addShutter, else results are undefined.
	
	void setMaxSpeed(float speed);
	
	void setSpeed(float speed);
	
	void retrieveSpeed();
	
	void saveSpeed();
	
	uint8_t checkEndOfRoad();
	
	void runSpeed();
	
	boolean isRunning();
	
	boolean isError();
	
    void moveTo(long absolute[]);
    
    /// Calls runSpeed() on all the managed shutters
    /// that have not acheived their target position.
    /// \return true if any shutter is still in the process of running to its target position.
    boolean run();

    /// Runs all managed shutters until they acheived their target position.
    /// Blocks until all that position is acheived. If you dont
    /// want blocking consider using run() instead.
    void    runSpeedToPosition();
	
	void	systemStatus();
	
	void 	systemStatusRawData();
	
	bool	shutterPositionNotEqual(uint8_t pos);
    
private:
    /// Array of pointers to the shutters we are controlling.
    /// Fills from 0 onwards
    shutter* _shutters[SHUTTERSYSTEM_MAX_SHUTTERS];

    /// Number of shutters we are controlling and the number
    /// of shutters in _shutters[]
    uint8_t       _num_shutters;
	
	float speed_array[SHUTTERSYSTEM_MAX_SHUTTERS];
};

/// @example ShutterSystem.pde
/// Use ShutterSystem class to manage multiple shutters and make them all move to 
/// the same position at the same time for linear 2d (or 3d) motion.

#endif
