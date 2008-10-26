#include "Parsers.hpp"
#include <boost/bind.hpp>
#include <vector>

using namespace std;
using namespace xmlpp;

typedef GenericLoader<Document> generic_loader;

enum COLOR { RED, GREEN, BLUE };

class Car :
    public Deserializable<Document>
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
    void LoadSpeed(speed_t& speed, const Document& d, const Node& n) const
    {
        Element speedNode = *n.FirstChildElement();
        istringstream ss( speedNode.Text() );
        ss >> speed;
        if ( speedNode.Value() == "mph" ) {
            speed *= 1.67;
        }
        else if ( speedNode.Value() != "kmh" ) {
            throw ParserError("Loader for element '" + speedNode.Value() + "' unspecified");
        }
    };

public:
    void Load(const Document& d, const Node& n)
    {
        generic_loader  loader;
        generic_loader  sizeLoader;

        loader.Attach("Vendor", &vendor);
        loader.Attach("Model", &model);

        sizeLoader.Attach("Length", &length);
        sizeLoader.Attach("Width", &width);
        sizeLoader.Attach("Height", &height);
        loader.Attach("Size", sizeLoader);

        loader.Attach("Mass", &mass);
        loader.Attach( "MaxSpeed", bind(&Car::LoadSpeed<float>, this, boost::ref(maxSpeed), _1, _2) );

        std::string col;
        loader.Attach("Color", &col);
        if ( col == "Red" ) {
            color = RED;
        }
        else if ( col == "Green" ) {
            color = GREEN;
        }
        else if ( col == "Blue" ) {
            color = BLUE;
        }

        // load
        loader(d,n);
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

void LoadCarAndAddToStore(vector<Car>& store, const Document& d, const Node& n)
{
    Car car;
    car.Load(d, n);
    store.push_back(car);
}

int main(void)
{
    // var
    Document        document;
    vector<Car>     cars;
    generic_loader  loader;

    loader.Attach( "Car", boost::bind(LoadCarAndAddToStore, boost::ref(cars), _1, _2) );

    document.SetFileSource("xml/Car.xml");
    loader( document, *document.FirstChildElement("Store") );

    for_each( cars.begin(), cars.end(), boost::bind(&Car::PrintInfo, _1) );

    return 0;
}

