
#if !defined(C___ERROR_CODES__H)
#define C___ERROR_CODES__H

/* types for errors */ 
typedef enum struc_return {
	Undefined = 0,
//
	Success		= 1,
	NothingToDo	= 2,
	RequestDeclined = 3,
//
	Failure		= -1,
	SysFailure	= -2,
	SysMemory	= -3,
	AppConfig	= -4,
	IOFailure	= -5,
	ParseFailure	= -6,
	PinError	= -7,
	DuplicateRec	= -8,
	NoDataFound	= -9,
	Ignore		= -10,
	MissingData = -12,
	BadSocket	= -13,
	ServerTimedOut	= -14,
	IOReadFailed	= -15,
	IOWriteFailed	= -16,
	NoMoreInput		= -17,
	ServerNotFound	= -18,
	ServerUnavailable	= -19,
	CanNotSetSocket = -20, 
	CannotConfigureConnection = -20, 
	CannotCreateSocket = -21
} Return_code;


#endif

