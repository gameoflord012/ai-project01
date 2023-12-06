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
    auto clazz = new MyClass();

    std::shared_ptr<MyClass> sharedPtr1 = std::shared_ptr<MyClass>(clazz);

    {
        std::shared_ptr<MyClass> sharedPtr2 = std::shared_ptr<MyClass>(sharedPtr1);
        std::cout << "Reference count: " << sharedPtr1.use_count() << std::endl;
    }

    // Getting the reference count
    std::cout << "Reference count: " << sharedPtr1.use_count() << std::endl;
}