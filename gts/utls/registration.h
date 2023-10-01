
/************************************************************************************
*                                                                                   *
*   Copyright (c) 2023 Xiaoqiang <username_nullptr@163.com>                         *
*                                                                                   *
*   This file is part of GTS3                                                       *
*   License: MIT License                                                            *
*                                                                                   *
*   Permission is hereby granted, free of charge, to any person obtaining a copy    *
*   of this software and associated documentation files (the "Software"), to deal   *
*   in the Software without restriction, including without limitation the rights    *
*   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       *
*   copies of the Software, and to permit persons to whom the Software is           *
*   furnished to do so, subject to the following conditions:                        *
*                                                                                   *
*   The above copyright notice and this permission notice shall be included in      *
*   all copies or substantial portions of the Software.                             *
*                                                                                   *
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      *
*   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        *
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     *
*   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          *
*   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   *
*   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE   *
*   SOFTWARE.                                                                       *
*                                                                                   *
*************************************************************************************/

#ifndef GTS_UTLS_REGISTRATION_H
#define GTS_UTLS_REGISTRATION_H

#include <rttr/registration>

#define _EXPAND_(...)  __VA_ARGS__

#define _ARG_OP_0(op,fixEnterArg,sep,arg )
#define _ARG_OP_1(op,fixEnterArg,sep,arg )       op(fixEnterArg, arg)
#define _ARG_OP_2(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_1(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_3(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_2(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_4(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_3(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_5(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_4(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_6(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_5(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_7(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_6(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_8(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_7(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_9(op,fixEnterArg,sep,arg, ...)   op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_8(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_10(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_9(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_11(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_10(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_12(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_11(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_13(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_12(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_14(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_13(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_15(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_14(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_16(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_15(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_17(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_16(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_18(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_17(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_19(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_18(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_20(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_19(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_21(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_20(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_22(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_21(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_23(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_22(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_24(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_23(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_25(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_24(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_26(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_25(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_27(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_26(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_28(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_27(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_29(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_28(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_30(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_29(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_31(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_30(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_32(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_31(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_33(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_32(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_34(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_33(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_35(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_34(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_36(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_35(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_37(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_36(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_38(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_37(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_39(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_38(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_40(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_39(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_41(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_40(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_42(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_41(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_43(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_42(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_44(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_43(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_45(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_44(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_46(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_45(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_47(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_46(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_48(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_47(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_49(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_48(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_50(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_49(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_51(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_50(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_52(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_51(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_53(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_52(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_54(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_53(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_55(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_54(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_56(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_55(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_57(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_56(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_58(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_57(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_59(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_58(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_60(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_59(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_61(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_60(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_62(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_61(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_63(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_62(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_64(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_63(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_65(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_64(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_66(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_65(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_67(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_66(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_68(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_67(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_69(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_68(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_70(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_69(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_71(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_70(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_72(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_71(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_73(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_72(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_74(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_73(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_75(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_74(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_76(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_75(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_77(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_76(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_78(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_77(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_79(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_78(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_80(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_79(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_81(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_80(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_82(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_81(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_83(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_82(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_84(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_83(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_85(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_84(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_86(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_85(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_87(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_86(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_88(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_87(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_89(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_88(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_90(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_89(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_91(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_90(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_92(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_91(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_93(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_92(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_94(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_93(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_95(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_94(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_96(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_95(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_97(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_96(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_98(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_97(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))
#define _ARG_OP_99(op,fixEnterArg,sep,arg, ...)  op(fixEnterArg, arg) sep _EXPAND_(_ARG_OP_98(op,fixEnterArg, _EXPAND_(sep),##__VA_ARGS__))

#define _ARG_SEQ() \
	99,98,97,96,95,94,93,92,91,90, \
	89,89,87,86,85,84,83,82,81,80, \
	79,78,77,76,75,74,73,72,71,70, \
	69,68,67,66,65,64,63,62,61,60, \
	59,58,57,56,55,54,53,52,51,50, \
	49,48,47,46,45,44,43,42,41,40, \
	39,38,37,36,35,34,33,32,31,30, \
	29,28,27,26,25,24,23,22,21,20, \
	19,18,17,16,15,14,13,12,11,10, \
	9,8,7,6,5,4,3,2,1,0

#define _ARG_N( \
	_0, _1,_2,_3,_4,_5,_6,_7,_8,_9,_10, \
	_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
	_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
	_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
	_41,_42,_43,_44,_45,_46,_47,_48,_49,_50, \
	_51,_52,_53,_54,_55,_56,_57,_58,_59,_60, \
	_61,_62,_63,_64,_65,_66,_67,_68,_69,_70, \
	_71,_72,_73,_74,_75,_76,_77,_78,_79,_80, \
	_81,_82,_83,_84,_85,_86,_87,_88,_89,_90, \
	_91,_92,_93,_94,_95,_96,_97,_98,_99, N, ...) N

#define _MACRO_CONCAT_(A,B)  A##B
#define _MACRO_CONCAT(A,B)   _MACRO_CONCAT_(A,B)
#define ARG_COUNT_(...)      _ARG_N(__VA_ARGS__)

#define ARG_COUNT(...) \
	ARG_COUNT_(unused, ##__VA_ARGS__, _ARG_SEQ())

#define _ARG_OP(...) \
	_MACRO_CONCAT(_ARG_OP_, ARG_COUNT(__VA_ARGS__))

#define _FOR_EACH(op, fe_arg, sep, arg, ...) \
	_ARG_OP(arg, ##__VA_ARGS__)(op, fe_arg, sep, arg, ##__VA_ARGS__)

#define _CHAIN_CALL(call, fe_arg, arg, ...) \
	_FOR_EACH(call, fe_arg, ., arg, ##__VA_ARGS__)

#define _RTTR_PROPERTY(T, s) \
	property(#s, &T::s)

#define GTS_REG_STRUCT(_class, arg, ...) \
	rttr::registration::class_<_class>(#_class).constructor([]{return _class();}). \
	_CHAIN_CALL(_RTTR_PROPERTY, _class, arg, ##__VA_ARGS__)


#endif //GTS_UTLS_REGISTRATION_H
