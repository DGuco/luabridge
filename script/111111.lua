#!/usr/bin/lua

function x11111_callfailedtest(x,y,i,j,world)
    local addNum = Add(x,y);
    local subNum = Sub(i,j);
    print("addNum = "..addNum)
    print("subNum = "..subNum)
    print("world = "..world)
    print("x11111_test1 done")
    return "1"
end


function x11111_test(x,y)
    local addNum = Add(x,y);
    local subNum = Sub(x,y);
    print("addNum = "..addNum)
    print("subNum = "..subNum)
    Say("Hello world");
    local testclass = OuterClass ()
    testclass:Say("Hello cpp");
    print("x11111_test done")
    return 1
end

function x11111_PrintG()
    print("{\t");
    for a,b in pairs(_G) do
        if (type(b) == "table") then
            print("\t",a,"\t",b,"\tlen = ",#b)
        else
            print("\t",a,"\t",b)
        end

        if  type(b) == "table" and tostring(a) ~= "_G" then
            for x,y in pairs(b) do
                if (type(y) == "table") then
                    print("\t\t","|--",x,y,"\tlen = ",#y)
                else
                    print("\t\t","|--",x,y)
                end
                if type(y) == "table" and tostring(x) ~= tostring(a) then
                    for x_,y_ in pairs(y) do
                        print("\t\t\t","|--",x_,y_)
                    end
                end
            end
        end
    end
    print("}\t");
    return 0
end
