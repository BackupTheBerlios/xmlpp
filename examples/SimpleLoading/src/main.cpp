#include "parsers.hpp"
#include <boost/bind.hpp>
#include <vector>
         
using namespace std;
using namespace xmlpp;

typedef generic_loader<xmlpp::document> document_loader;

enum COLOR { RED, GREEN, BLUE };

class Car
{
private:
    std::string     vendor;
    std::string     model;

    // size
    int     width;
    int     length;
    int     height;
    float   mass;

    // other
    float   maxSpeed;
    COLOR   color;

    template<class speed_t>
    void LoadSpeed(speed_t& speed, const document& /*d*/, const node& n) const
    {
        element speedNode = *n.first_child_element();
        istringstream ss( speedNode.get_text() );
        ss >> speed;
        if ( speedNode.get_value() == "mph" ) {
            speed *= 1.67f;
        }
        else if ( speedNode.get_value() != "kmh" ) {
            throw parser_error("Loader for element '" + speedNode.get_value() + "' unspecified");
        }
    };

public:
    void Load(const document& d, const node& n)
    {
        document_loader  loader;
        document_loader  sizeLoader;

        loader.attach("Vendor", &vendor);
        loader.attach("Model", &model);

        sizeLoader.attach("Length", &length );
        sizeLoader.attach("Width", &width );
        sizeLoader.attach("Height", &height );
        loader.attach("Size", sizeLoader);

        loader.attach("Mass", &mass);
        loader.attach( "MaxSpeed", boost::bind(&Car::LoadSpeed<float>, this, boost::ref(maxSpeed), _1, _2) );

        std::string col;
        loader.attach("Color", &col);

        // load
        loader(d,n);

        if ( col == "Red" ) {
            color = RED;
        }
        else if ( col == "Green" ) {
            color = GREEN;
        }
        else if ( col == "Blue" ) {
            color = BLUE;
        }
    }

    void PrintInfo() const
    {
        cout << vendor << " " << model << endl
             << "Size: " << endl
             << "  length = " << length / 1000.0 << "m" << endl
             << "  width = " << width / 1000.0 << "m" << endl
             << "  height = " << height / 1000.0 << "m" << endl
             << "Mass: " << mass << "kg" << endl
             << "Maximum speed: " << maxSpeed << "km/h" << endl << endl;
    }
};

void LoadCarAndAddToStore(vector<Car>& store, const document& d, const node& n)
{
    Car car;
    car.Load(d, n);
    store.push_back(car);
}
        
int main(void)
{   
    // var
    document         doc;
    vector<Car>      cars;
    document_loader  loader;

    loader.attach( "Car", boost::bind(LoadCarAndAddToStore, boost::ref(cars), _1, _2) );

    doc.set_file_source("xml/Car.xml");
    loader( doc, *doc.first_child_element("Store") );

    for_each( cars.begin(), cars.end(), boost::bind(&Car::PrintInfo, _1) );
      
    return 0;
}

