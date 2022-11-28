#pragma once
#include "../DataStructures/gcArray.h"

namespace object {
	class Obj;
}

enum class ValueType {
	NUM,
	BOOL,
	OBJ,
	NIL
};

struct Value {
	ValueType type;
	union {
		double number;
		bool boolean;
		object::Obj* object;
	} as;
	Value() {
		type = ValueType::NIL;
		as.object = nullptr;
	}

	Value(double num) {
		type = ValueType::NUM;
		as.number = num;
	}

	Value(bool _bool) {
		type = ValueType::BOOL;
		as.boolean = _bool;
	}

	Value(object::Obj* _object) {
		type = ValueType::OBJ;
		as.object = _object;
	}
	
	static Value nil() {
		return Value();
	}

	bool equals(Value other) {
		return false;
	}
};

enum class OpCode {
	//Helpers
	POP,
	POPN,
	//constants
	CONSTANT,
	CONSTANT_LONG,
	NIL,
	TRUE,
	FALSE,
	//unary
	NEGATE,
	NOT,
	BIN_NOT,
	//binary
	BITWISE_XOR,
	BITWISE_OR,
	BITWISE_AND,
	ADD,
	SUBTRACT,
	MULTIPLY,
	DIVIDE,
	MOD,
	BITSHIFT_LEFT,
	BITSHIFT_RIGHT,
	ADD_1,
	SUBTRACT_1,
	//comparisons and equality
	EQUAL,
	NOT_EQUAL,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL,
	//Statements
	PRINT,
	TO_STRING,
	//Variables
	DEFINE_GLOBAL,
	DEFINE_GLOBAL_LONG,
	GET_GLOBAL,
	GET_GLOBAL_LONG,
	SET_GLOBAL,
	SET_GLOBAL_LONG,
	GET_LOCAL,
	SET_LOCAL,
	GET_UPVALUE,
	SET_UPVALUE,
	CLOSE_UPVALUE,
	//Arrays
	CREATE_ARRAY,
	GET,
	SET,
	//control flow
	JUMP,
	JUMP_IF_FALSE,
	JUMP_IF_TRUE,
	JUMP_IF_FALSE_POP,
	LOOP,
	JUMP_POPN,
	SWITCH,

	//Functions
	CALL,
	RETURN,
	CLOSURE,
	CLOSURE_LONG,

	//OOP
	CLASS,
	GET_PROPERTY,
	GET_PROPERTY_LONG,
	SET_PROPERTY,
	SET_PROPERTY_LONG,
	CREATE_STRUCT,
	CREATE_STRUCT_LONG,
	METHOD,
	INVOKE,
	INVOKE_LONG,
	INHERIT,
	GET_SUPER,
	GET_SUPER_LONG,
	SUPER_INVOKE,
	SUPER_INVOKE_LONG,

	//fibers
	FIBER_CREATE,
	FIBER_RUN,
	FIBER_YIELD,
};


struct codeLine {
	uInt64 end;
	uInt64 line;
	string name;

	codeLine() {
		line = 0;
		end = 0;
		name = "";
	}
	codeLine(uInt64 _line, string& _name) {
		line = _line;
		name = _name;
	}
};

class Chunk {
public:
	ManagedArray<codeLine> lines;

	ManagedArray<uint8_t> code;
	ManagedArray<Value> constants;
	Chunk() {};
	void writeData(uint8_t opCode, uInt line, string& name);
	codeLine getLine(uInt offset);
	void disassemble(string name);
	uInt addConstant(Value val);
};