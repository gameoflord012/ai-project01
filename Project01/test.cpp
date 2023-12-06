#include <iostream>
#include <memory>

class MyClass {
public:
    MyClass() {
        std::cout << "Constructor called." << std::endl;
    }

    ~MyClass() {
        std::cout << "Destructor called." << std::endl;
    }
};

int main() {
    // Creating a shared_ptr (reference count = 1)
    std::shared_ptr<MyClass> sharedPtr = std::make_shared<MyClass>();

    // Getting the reference count
    std::cout << "Reference count: " << sharedPtr.use_count() << std::endl;

    {
        // Creating another shared_ptr (reference count = 2)
        std::shared_ptr<MyClass> sharedPtr2 = sharedPtr;

        // Getting the reference count
        std::cout << "Reference count: " << sharedPtr.use_count() << std::endl;
        std::cout << "Reference count for sharedPtr2: " << sharedPtr2.use_count() << std::endl;

    } // sharedPtr2 goes out of scope, reference count decreases to 1

    // Getting the reference count
    std::cout << "Reference count: " << sharedPtr.use_count() << std::endl;

} // sharedPtr goes out of scope, reference count decreases to 0, destructor of MyClass is called
