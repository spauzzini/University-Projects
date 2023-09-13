--Countries with one (or more) offices that did not sell a single item during the
--year 2003. The output should show each country and the number of offices that
--did not sell a single item in the year 2003. Order the output by the number
--of offices. The country with higher number of ”no selling offices” should be in
--the first line.


select distinct ofi.country, count(o.ordernumber)        -- Elements to be selected and printed (count gives us the
                                                         -- total amount of orders and we have put distinct to take
                                                         -- those which are different)

from offices ofi, employees e, customers c, orders o     -- Tables from where we want to select our results

where extract(year from o.orderdate) = '2003'            -- Conditions to satisfy to get the results we want expressed
	and ofi.officecode = e.officecode                    -- and facts because in this case is easier. First we want to
	and e.employeenumber = c.salesrepemployeenumber      -- get the total amount of orders for each country in 2003
	and c.customernumber = o.customernumber 

group by ofi.country                                     -- To group the results of the ordernumbers counted

having count(o.ordernumber) = 0                          -- Condition to print those countries with no orders in 2003
