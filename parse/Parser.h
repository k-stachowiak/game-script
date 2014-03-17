#pragma once

namespace moon {
namespace parse {

	class CParser
	{
	public:
		virtual std::unique_ptr<itpr::CScope> Parse(const std::string& source) const = 0;
	};

}
}

