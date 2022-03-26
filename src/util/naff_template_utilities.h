template<class T, typename ...InitArgs>
class Instance final : public T
{
public:
	Instance(const InitArgs&... Args) : T(Args...) {}
};