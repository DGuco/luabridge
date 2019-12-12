local_upvalue = 100

function x11111_test()
    local x = 1
    local y = 2
    local addNum = Add(x,nil);
    print("addNum = "..addNum)
    LuaFnAdd(x);
    return addNum;
end

