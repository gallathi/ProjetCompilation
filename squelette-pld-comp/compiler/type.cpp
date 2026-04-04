#include "type.h"

int TypeSizes::getTypeSize(Type t) {
	switch(t) {
		case INT:
			return 4;
			break;
		case VOID:
			return 8;
			break;
		case CHAR:
			return 1;
			break;
		case DOUBLE:
			return 8;
			break;
	}
	return 1;
}
