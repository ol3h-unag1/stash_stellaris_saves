












namespace Communication
{

class Writer
{
    static std::size_t object_counter;

public:
    Writer();
    
    Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = delete;
    Writer& operator=(Writer&&) = delete;

public:
    void fork_children();

private:
    std::size_t obj_id = 9001;
};

}