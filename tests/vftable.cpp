#include "gtest/gtest.h"

namespace {

	class IBase {
	public:
		virtual ~IBase(void){}
		virtual void proc( void ) = 0;
		
		int value_01;
		int value_02;
	};

	
	class IBase2 {
	public:
		virtual ~IBase2(void){}
		virtual void proc( void ) = 0;
		int value_04;

	};
	class Derived : public IBase {
	public:
		Derived( int value ) : value_03(value){}
		virtual ~Derived(void) {

		}
		
		virtual void proc(void) {

		}
		int value_03;
	};


	
	class Derived2 : public IBase , public IBase2 {
	public:
		Derived2( int value ) : value_03(value){
			value_04 = 4;
		}
		virtual ~Derived2(void) {
			
		}
		
		virtual void proc(void) {

		}
		int value_03;
	};

	class Derived3 : public IBase {
		Derived3( int value ) : value_03(value){
		}
		virtual ~Derived3(void) {
			
		}
		
		virtual void proc(void) {

		}
		int value_03;
	};

}

TEST(test_on_runtime, vftable) {
	IBase* ibase = new Derived(3);
	ibase->value_01 = 1;
	ibase->value_02 = 2;

	/*
	vftable | value_01 | value_02 | value_03
	vftable -> [ dtor , proc ]
	*/

}

TEST(test_on_runtime, vftable1) {
	Derived2* derived = new Derived2(3);
	IBase2* ibase2 = derived;
	IBase* ibase = derived;
	ibase->value_01 = 1;
	ibase->value_02 = 2;

	Derived2* from_ibase = static_cast< Derived2* >(ibase);
	Derived2* from_ibase2 = static_cast< Derived2* >(ibase2);

	/*
	memory layout
	[ IBase_vftable  | IBase::value_01  | IBase::value_02 ] 
	[ IBase2_vftable | IBase2::value_04 ] 
	[ Derived2::value_03 ]
	*/
	// check in compile time
	Derived3* wrong_cast = static_cast<Derived3*>(ibase);
	// check in runtime
	Derived3* right_cast = dynamic_cast<Derived3*>(ibase);
}