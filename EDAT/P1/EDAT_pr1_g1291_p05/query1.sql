-- List the total amount of money paid by the customers that have ordered the
-- “1940 Ford Pickup Truck” (the money may have been paid to purchase other
-- items in addition to this one). Order the result by the amount of money paid.
-- Use descending order. Each row should show customernumber, customername
-- and total amount


select 
       c.customernumber, c.customername, pa.amount                      -- Elements to be selected and printed
from 
       customers c join orders o on c.customernumber = o.customernumber -- In this lines we have established
       join orderdetails od on o.ordernumber=od.ordernumber             -- the relations between the tables and they
       join products p on od.productcode=p.productcode                  -- primary and foreign keys using joins and on
       join payments pa on c.customernumber = pa.customernumber         -- because they are not repeated

where p.productname='1940 Ford Pickup Truck'                            -- Condition to select the costumers

order by pa.amount desc                                                 -- To order the amounts in descending order
