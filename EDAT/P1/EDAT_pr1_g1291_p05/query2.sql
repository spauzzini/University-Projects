--Average time between order date and ship date for each product line. Each
--row should show: the productline and the corresponding average time


select
	pl.productline, avg(o.shippeddate - o.orderdate) as avg_time       -- Elements to be selected and printed (avg_time
	                                                                   -- gives us the average between the shipped
	                                                                   -- date and the order date)
from 
	productlines pl join products p on pl.productline = p.productline -- In this lines we have established
	join orderdetails od on p.productcode = od.productcode            -- the relations between the tables and they
	join orders o on od.ordernumber = o.ordernumber                   -- primary and foreign keys using joins and on
	                                                                  -- because they are not repeated

group by pl.productline                                               -- To group the average
