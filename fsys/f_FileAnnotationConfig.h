#ifndef __INCLUDE_f_FileAnnotationConfig__HH
#define __INCLUDE_f_FileAnnotationConfig__HH

#pragma once
#include <util/EnvironmentStd.h>
#include <fsys/f_FileName.h>
#include <ctype/c_Types.h>

#define RecordAnnotation char

struct f_FileAnnotationConfig 
{
	f_FileAnnotationConfig() 
		: ann_Null(0)
	{
		reset();
	}
	RecordAnnotation getRecType(char * line) {
		if (!line )
			return ann_Null;
		return line[0];
	}
	void setTarget(RecordAnnotation target)
	{
		this->m_target = target;
	}
	bool isTarget(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isTarget(test));
	}
	bool isTarget(RecordAnnotation test) {
		return (test == m_target);
	}

	bool isComment(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isComment(test));
	}
	bool isComment(RecordAnnotation test) {
		return (test == ann_Comment);
	}

	bool isDone(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isDone(test));
	}
	bool isDone(RecordAnnotation test) {
		return (test == ann_Done);
	}

	bool isOpen(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isOpen(test));
	}
	bool isOpen(RecordAnnotation test) {
		return (test == ann_Open);
	}

	bool isError(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isError(test));
	}
	bool isError(RecordAnnotation test) {
		return (test == ann_Error);
	}

	bool isDuplicate(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isDuplicate(test));
	}
	bool isDuplicate(RecordAnnotation test) {
		return (test == ann_Duplicate);
	}

	bool isLoadError(char * line) {
		RecordAnnotation test = getRecType(line);
		return (isLoadError(test));
	}
	bool isLoadError(RecordAnnotation test) {
		return (test == ann_LoadError);
	}

	bool isUnknown(char * line) {
		RecordAnnotation test = getRecType(line);
		return isUnknown(test);
	}

	bool filterIsSet()
	{
		return (m_target != ann_All && !isUnknown(m_target) );
	}

	bool isAnnotated(char * line) {
		RecordAnnotation test = getRecType(line);
		return (
			test == ann_Open || 
			test == ann_Done || 
			test == ann_Duplicate || 
			test == ann_Error || 
			test == ann_Comment || 
			test == ann_LoadError );
	}
	bool isUnknown(RecordAnnotation test) {
		return (
			test != ann_Open && 
			test != ann_Done && 
			test != ann_Duplicate && 
			test != ann_Error && 
			test != ann_All && 
			test != ann_Comment && 
			test != ann_LoadError );
	}
	
//		void setOpenAnnotation(RecordAnnotation ra) { this->
	void clear()
	{
		ann_Open		= 0;
		ann_Done		= 0;
		ann_Duplicate	= 0;
		ann_Error		= 0;
		ann_All			= 0;
		ann_Comment		= 0;
		ann_LoadError= 0;
		m_target = ann_All;  //records to work on - filter all others
	}
	bool valid()
	{
		return(
			! isUnknown(ann_Open		) &&
			! isUnknown(ann_Done		) &&
			! isUnknown(ann_Duplicate	) &&
			! isUnknown(ann_Error		) &&
			! isUnknown(ann_All			) &&
			! isUnknown(ann_Comment		) &&
			! isUnknown(ann_LoadError) );
	}

	void set(f_FileAnnotationConfig ac)
	{
		ann_Open		= ac.ann_Open;
		ann_Done		= ac.ann_Done;
		ann_Duplicate	= ac.ann_Duplicate;
		ann_Error		= ac.ann_Error;
		ann_All			= ac.ann_All;
		ann_Comment		= ac.ann_Comment;
		ann_LoadError = ac.ann_LoadError;
	}

	void reset()
	{
		ann_Open		= '_';
		ann_Done		= '.';
		ann_Duplicate	= '=';
		ann_Error		= '!';
		ann_All			= '*';
		ann_Comment		= '#';
		ann_LoadError = '+';
		m_target = ann_All;  //records to work on - filter all others
	}
	void dump(ostream& out)
	{
		out 
			<< "Target(" << m_target << "), "
			<< "ann_Open(" << ann_Open << "), "
			<< "ann_Done(" << ann_Done << "), "
			<< "ann_Duplicate(" << ann_Duplicate << "), "
			<< "ann_Error(" << ann_Error << "), "
			<< "ann_All(" << ann_All << "), "
			<< "ann_Comment(" << ann_Comment << "), "
			<< "ann_LoadError(" << ann_LoadError << ")"
			<< endl;
	}

	const RecordAnnotation ann_Null;
	RecordAnnotation m_target;  
	RecordAnnotation ann_Open;	
	RecordAnnotation ann_Done;	
	RecordAnnotation ann_Duplicate;
	RecordAnnotation ann_Error;
	RecordAnnotation ann_All;
	RecordAnnotation ann_Comment;
	RecordAnnotation ann_LoadError;
};

#endif

