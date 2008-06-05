/*! \file
    \brief Definition of the unit test for the \ref obj_factory_t

*/

/* system include */
/* local include */
#include "neoip_obj_factory_nunit.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_serial.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;



class nunit_obj_factory_product_base_t {
public:
	virtual	~nunit_obj_factory_product_base_t(){}
	virtual int              getValue()          = 0;
	virtual void             setValue(int value) = 0;

	FACTORY_BASE_CLASS_DECLARATION(nunit_obj_factory_product_base_t);
};
   

class nunit_obj_factory_product_1 : public nunit_obj_factory_product_base_t {
private:
	int m_value;
public:
	virtual ~nunit_obj_factory_product_1(){}
	int  getValue()          {return m_value;}
	void setValue(int value) {m_value = value;}
	FACTORY_PRODUCT_DECLARATION(nunit_obj_factory_product_base_t, nunit_obj_factory_product_1);
};


class nunit_obj_factory_product_2 : public nunit_obj_factory_product_base_t {
private:
	int m_value;
public:
	virtual ~nunit_obj_factory_product_2(){}
	int  getValue()          {return m_value*100;}
	void setValue(int value) {m_value = value;}
	FACTORY_PRODUCT_DECLARATION(nunit_obj_factory_product_base_t, nunit_obj_factory_product_2);
};

FACTORY_PLANT_DECLARATION(nunit_obj_factory_plant, std::string, nunit_obj_factory_product_base_t);
FACTORY_PLANT_DEFINITION (nunit_obj_factory_plant, std::string, nunit_obj_factory_product_base_t);

FACTORY_PRODUCT_DEFINITION(nunit_obj_factory_product_base_t, nunit_obj_factory_product_1);
FACTORY_PRODUCT_DEFINITION(nunit_obj_factory_product_base_t, nunit_obj_factory_product_2);


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	obj_factory_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");

	// insert all the factory_product_t
	FACTORY_PRODUCT_INSERT	(nunit_obj_factory_plant, std::string, nunit_obj_factory_product_base_t
						, "two", nunit_obj_factory_product_2);
	FACTORY_PRODUCT_INSERT	(nunit_obj_factory_plant, std::string, nunit_obj_factory_product_base_t
						, "one", nunit_obj_factory_product_1);

	// test if the factory_product_t have been inserted
	std::vector<std::string>	key_list = nunit_obj_factory_plant->get_key_list();
	for(size_t i = 0; i < key_list.size(); i++ ){
		KLOG_ERR("i=" << i << " key=" << key_list[i]);
	}
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

