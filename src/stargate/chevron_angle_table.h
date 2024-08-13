#pragma once

constexpr unsigned short CalcChevronAngle(unsigned short ChevronNumber)
{
	//well this is obviously wrong.
	return 360 / ChevronNumber;
}

static constexpr unsigned short ChevronAngleTable[39]
{
    CalcChevronAngle(1),
    CalcChevronAngle(2),
    CalcChevronAngle(3),
    CalcChevronAngle(4),
    CalcChevronAngle(5),
    CalcChevronAngle(6),
    CalcChevronAngle(7),
    CalcChevronAngle(8),
    CalcChevronAngle(9),
    CalcChevronAngle(10),
    CalcChevronAngle(11),
    CalcChevronAngle(12),
    CalcChevronAngle(13),
    CalcChevronAngle(14),
    CalcChevronAngle(15),
    CalcChevronAngle(16),
    CalcChevronAngle(17),
    CalcChevronAngle(18),
    CalcChevronAngle(19),
    CalcChevronAngle(20),
    CalcChevronAngle(21),
    CalcChevronAngle(22),
    CalcChevronAngle(23),
    CalcChevronAngle(24),
    CalcChevronAngle(25),
    CalcChevronAngle(26),
    CalcChevronAngle(27),
    CalcChevronAngle(28),
    CalcChevronAngle(29),
    CalcChevronAngle(30),
    CalcChevronAngle(31),
    CalcChevronAngle(32),
    CalcChevronAngle(33),
    CalcChevronAngle(34),
    CalcChevronAngle(35),
    CalcChevronAngle(36),
    CalcChevronAngle(38),
    CalcChevronAngle(39)
};