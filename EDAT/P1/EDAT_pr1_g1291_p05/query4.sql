--Office that sold most products (number of items). Note that in a single order
--many items, even of the same product, may be sold. The output should show
--the “officecode” and the number of items sold.


select ofi.officecode, sum (od.quantityordered)                       -- Elements to be selected and printed (sum gives
                                                                      -- us the total amount of elements)

from offices ofi join employees e on ofi.officecode = e.officecode    -- In this lines we have established
    join customers c on e.employeenumber = c.salesrepemployeenumber   -- the relations between the tables and they
    join orders o on c.customernumber = o.customernumber              -- primary and foreign keys using joins and on
    join orderdetails od on o.ordernumber = od.ordernumber            -- because they are not repeated
    join products p on p.productcode=od.productcode

group by ofi.officecode                                               -- To group the quantities ordered

order by sum (od.quantityordered) desc                                -- To order our results in descending order

limit 1                                                               -- To only appear one result in our table,
                                                                      -- the biggest amount
