namespace llec
{
    template <class... Ts>
    struct overload : Ts...
    {
        using Ts::operator()...;
    };
} // namespace llec