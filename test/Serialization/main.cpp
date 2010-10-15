#include "serialization/serialization.hpp"

#define BOOST_TEST_MODULE SerializationTest
#include <boost/test/unit_test.hpp>

struct ability
{
    enum TYPE
    {
        MAKE_TEA,
        FLY,
        SUPER_STRIKE,
        MEGA_STRIKE,
        BRUTAL_STRIKE
    };

    TYPE        type;
    unsigned    selfDamage;
    unsigned    splashDamage;
    unsigned    directDamage;
    unsigned    powerConsumption;

    ability() {}
    ability(TYPE        type_,
            unsigned    selfDamage_,
            unsigned    splashDamage_,
            unsigned    directDamage_,
            unsigned    powerConsumption_) :
        type(type_),
        selfDamage(selfDamage_),
        splashDamage(splashDamage_),
        directDamage(directDamage_),
        powerConsumption(powerConsumption_)
    {}

    bool operator == (const ability& rhs) const
    {
        return (type == rhs.type)
                && (selfDamage == rhs.selfDamage)
                && (splashDamage == rhs.splashDamage)
                && (directDamage == rhs.directDamage)
                && (powerConsumption == rhs.powerConsumption);
    }
    
    bool operator != (const ability& rhs) const
    {
        return !(*this == rhs);
    }

    void serialize( xmlpp::document&    d,
                    xmlpp::element&     n,
                    xmlpp::s_state      s )
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("type",               xmlpp::as_attribute((int&)type));
        serializer &= xmlpp::make_nvp("self_damage",        xmlpp::as_text(selfDamage));
        serializer &= xmlpp::make_nvp("splash_damage",      xmlpp::as_text(splashDamage));
        serializer &= xmlpp::make_nvp("direct_damage",      xmlpp::as_text(directDamage));
        serializer &= xmlpp::make_nvp("power_consumption",  xmlpp::as_text(powerConsumption));
        serializer.serialize(d, n, s);
    }

    XMLPP_STORE_AS(xmlpp::element);
    XMLPP_SERIALIZATION_MERGE_MEMBER(ability, xmlpp::document)
};

struct superman
{
    std::vector<ability>    abilities;
    std::string             name;
    unsigned                hp;
    unsigned                superPower;

    bool operator == (const superman& rhs) const
    {
        if ( abilities.size() != rhs.abilities.size() ) {
            return false;
        }

        for (size_t i = 0; i<abilities.size(); ++i) 
        {
            if (abilities[i] != rhs.abilities[i]) {
                return false;
            }
        }

        return (name == rhs.name)
                && (hp == rhs.hp)
                && (superPower == rhs.superPower);
    }

    void serialize( xmlpp::document&    d,
                    xmlpp::element&     n,
                    xmlpp::s_state      s )
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("name",        xmlpp::as_attribute(name));
        serializer &= xmlpp::make_nvp("hp",          xmlpp::as_text(hp));
        serializer &= xmlpp::make_nvp("super_power", xmlpp::as_text(superPower));
        serializer &= xmlpp::make_nvp("abilities",   xmlpp::make_nvp("ability", xmlpp::as_element_set(abilities)) );
        serializer.serialize(d, n, s);
    }

    XMLPP_STORE_AS(xmlpp::element);
    XMLPP_SERIALIZATION_MERGE_MEMBER(superman, xmlpp::document)
};

// basic serialization test
BOOST_AUTO_TEST_CASE(serialization_test_0)
{
    superman sm[2];
    sm[0].abilities.push_back( ability(ability::FLY, 0, 0, 0, 10) );
    sm[0].abilities.push_back( ability(ability::SUPER_STRIKE, 0, 10, 100, 10) );
    sm[0].abilities.push_back( ability(ability::MEGA_STRIKE, 10, 100, 1000, 100) );
    sm[0].abilities.push_back( ability(ability::BRUTAL_STRIKE, 100, 1000, 10000, 1000) );
    sm[0].abilities.push_back( ability(ability::MAKE_TEA, 99999, 99999, 0, 99999) );
    sm[0].name = "Dan";
    sm[0].hp = 10000;
    sm[0].superPower = 1000000;

    xmlpp::document document;
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "superman", xmlpp::as_element(sm[0]) );
        serializer.save(document);
    }

    document.get_tixml_document()->Print();

    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "superman", xmlpp::as_element(sm[1]) );
        serializer.load(document);
    }

    BOOST_CHECK(sm[0] == sm[1]);
}

struct monster
{
    XMLPP_STORE_AS(xmlpp::element);

    enum TYPE
    {
        TEAPOT,
        STONE_GIANT,
        GODZILLA
    };

    virtual TYPE get_type() = 0;

    virtual void serialize( xmlpp::document&    d,
                            xmlpp::element&     n,
                            xmlpp::s_state      s ) = 0;

    XMLPP_SERIALIZATION_MERGE_MEMBER(monster, xmlpp::document)

    virtual ~monster() {}
};

struct teapot :
    public monster
{
    TYPE get_type() { return TEAPOT; }

    void serialize( xmlpp::document&    d,
                    xmlpp::element&     n,
                    xmlpp::s_state      s )
    {}
};

struct stone_giant :
    public monster
{
    TYPE get_type() { return STONE_GIANT; }

    unsigned                level;
    unsigned                hp;
    unsigned                superPower;
    std::vector<ability>    abilities;

    bool operator == (const stone_giant& rhs) const
    {
        if ( abilities.size() != rhs.abilities.size() ) {
            return false;
        }

        for (size_t i = 0; i<abilities.size(); ++i) 
        {
            if (abilities[i] != rhs.abilities[i]) {
                return false;
            }
        }

        return (level == rhs.level)
                && (hp == rhs.hp)
                && (superPower == rhs.superPower);
    }

    void serialize( xmlpp::document&    d,
                    xmlpp::element&     n,
                    xmlpp::s_state      s )
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("level",       xmlpp::as_text(level));
        serializer &= xmlpp::make_nvp("hp",          xmlpp::as_text(hp));
        serializer &= xmlpp::make_nvp("super_power", xmlpp::as_text(superPower));
        serializer &= xmlpp::make_nvp("abilities",   xmlpp::make_nvp("ability", xmlpp::as_element_set(abilities)) );
        serializer.serialize(d, n, s);
    }
};

struct godzilla :
    public monster
{
    TYPE get_type() { return GODZILLA; }

    unsigned                level;
    unsigned                hp;
    unsigned                superPower;
    std::vector<ability>    abilities;

    bool operator == (const godzilla& rhs) const
    {
        if ( abilities.size() != rhs.abilities.size() ) {
            return false;
        }

        for (size_t i = 0; i<abilities.size(); ++i) 
        {
            if (abilities[i] != rhs.abilities[i]) {
                return false;
            }
        }

        return (level == rhs.level)
                && (hp == rhs.hp)
                && (superPower == rhs.superPower);
    }

    void serialize( xmlpp::document&    d,
                    xmlpp::element&     n,
                    xmlpp::s_state      s )
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("level",       xmlpp::as_text(level));
        serializer &= xmlpp::make_nvp("hp",          xmlpp::as_text(hp));
        serializer &= xmlpp::make_nvp("super_power", xmlpp::as_text(superPower));
        serializer &= xmlpp::make_nvp("abilities",   xmlpp::make_nvp("ability", xmlpp::as_element_set(abilities)) );
        serializer.serialize(d, n, s);
    }
};

// check mostly container serialization
BOOST_AUTO_TEST_CASE(serialization_test_1)
{
    std::vector< boost::shared_ptr<monster> > foes[2];
    
    {
        boost::shared_ptr<godzilla> foe(new godzilla);
        foe->level      = 1;
        foe->hp         = 100;
        foe->superPower = 200;
        foe->abilities.push_back( ability(ability::SUPER_STRIKE, 0, 10, 100, 10) );
        foe->abilities.push_back( ability(ability::MEGA_STRIKE, 10, 100, 1000, 100) );
        foes[0].push_back(foe);
    }
    
    {
        boost::shared_ptr<godzilla> foe(new godzilla);
        foe->level      = 2;
        foe->hp         = 100;
        foe->superPower = 200;
        foe->abilities.push_back( ability(ability::SUPER_STRIKE, 0, 10, 100, 10) );
        foe->abilities.push_back( ability(ability::MEGA_STRIKE, 10, 100, 1000, 100) );
        foes[0].push_back(foe);
    }

    {
        boost::shared_ptr<stone_giant> foe(new stone_giant);
        foe->level      = 1;
        foe->hp         = 100;
        foe->superPower = 500;
        foe->abilities.push_back( ability(ability::MEGA_STRIKE, 10, 100, 1000, 100) );
        foes[0].push_back(foe);
    }

    {
        boost::shared_ptr<stone_giant> foe(new stone_giant);
        foe->level      = 2;
        foe->hp         = 100;
        foe->superPower = 500;
        foe->abilities.push_back( ability(ability::MEGA_STRIKE, 10, 100, 1000, 100) );
        foes[0].push_back(foe);
    }

    {
        boost::shared_ptr<teapot> foe(new teapot);
        foes[0].push_back(foe);
    }

    xmlpp::document document;
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "godzilla",      xmlpp::as_element_set<godzilla>(foes[0]) );
        serializer &= xmlpp::make_nvp( "stone_giant",   xmlpp::as_element_set<stone_giant>(foes[0]) );
        serializer &= xmlpp::make_nvp( "teapot",        xmlpp::as_element_set<teapot>(foes[0]) );
        serializer.save(document);
    }

    document.get_tixml_document()->Print();

    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "godzilla",      xmlpp::as_element_set<godzilla>(foes[1]) );
        serializer &= xmlpp::make_nvp( "stone_giant",   xmlpp::as_element_set<stone_giant>(foes[1]) );
        serializer &= xmlpp::make_nvp( "teapot",        xmlpp::as_element_set<teapot>(foes[1]) );
        serializer.load(document);
    }
    
    BOOST_CHECK_EQUAL( foes[0].size(), foes[1].size() );
    for (size_t i = 0; i<foes[0].size(); ++i)
    {
        BOOST_CHECK_EQUAL( foes[0][i]->get_type(), foes[1][i]->get_type() );
        switch ( foes[0][i]->get_type() )
        {
        case monster::GODZILLA:
            BOOST_CHECK( static_cast<godzilla&>(*foes[0][i]) == static_cast<godzilla&>(*foes[1][i]) );
            break;

        case monster::STONE_GIANT:
            BOOST_CHECK( static_cast<stone_giant&>(*foes[0][i]) == static_cast<stone_giant&>(*foes[1][i]) );
            break;
            
        case monster::TEAPOT:
            break;

        default:
            // do nothing
            assert(!"Invalid foe type");
        }
    }
}

struct helicopter
{
    void load( const xmlpp::document&    d,
               const xmlpp::element&     n)
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("name",			xmlpp::from_attribute(name));
        serializer &= xmlpp::make_nvp("max_speed",      xmlpp::from_text(maxSpeed));
        serializer &= xmlpp::make_nvp("mass",		    xmlpp::from_text(mass));
        serializer &= xmlpp::make_nvp("max_passengers", xmlpp::from_text(maxPassengers));
        serializer.load(d, n);
    }

    void save( xmlpp::document&    d,
               xmlpp::element&     n) const
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp("name",			xmlpp::to_attribute(name));
        serializer &= xmlpp::make_nvp("max_speed",      xmlpp::to_text(maxSpeed));
        serializer &= xmlpp::make_nvp("mass",		    xmlpp::to_text(mass));
        serializer &= xmlpp::make_nvp("max_passengers", xmlpp::to_text(maxPassengers));
        serializer.save(d, n);
    }
	
    bool operator == (const helicopter& rhs) const
    {
		return (name == rhs.name) && (maxSpeed == rhs.maxSpeed) && (mass == rhs.mass) && (maxPassengers == rhs.maxPassengers);
	}

	std::string name;
	float		maxSpeed;
	float		mass;
	int			maxPassengers;
};

// test to/from serialization functions
BOOST_AUTO_TEST_CASE(serialization_test_2)
{
    std::vector<helicopter> helicopters[2];

	helicopter ka50;
	{
		ka50.name			= "Ка-50";
		ka50.maxSpeed       = 315.0f;
		ka50.mass           = 7700.0f;
		ka50.maxPassengers  = 1;
	}
	helicopters[0].push_back(ka50);
    
	helicopter mi8;
	{
		mi8.name		   = "Ми-8П";
		mi8.maxSpeed       = 250.0f;
		mi8.mass           = 7000.0f;
		mi8.maxPassengers  = 31;
	}
	helicopters[0].push_back(mi8);

    xmlpp::document document;
    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "helicopters",   xmlpp::to_element_set(helicopters[0]) );
        serializer.save(document);
    }

    document.get_tixml_document()->Print();

    {
        xmlpp::generic_serializer<xmlpp::document> serializer;
        serializer &= xmlpp::make_nvp( "helicopters",   xmlpp::from_element_set(helicopters[1]) );
        serializer.load(document);
    }
    
    BOOST_CHECK_EQUAL( helicopters[0].size(), helicopters[1].size() );
    for (size_t i = 0; i<helicopters[0].size(); ++i) {
        BOOST_CHECK( helicopters[0][i] == helicopters[1][i] );
    }
}
