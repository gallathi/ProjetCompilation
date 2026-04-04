
#ifndef TYPE_H
#define TYPE_H

enum Type
{
    INT,
    VOID,
    CHAR,
    DOUBLE
};

class TypeSizes {
	public:
		int getTypeSize(Type t);
};

#endif
