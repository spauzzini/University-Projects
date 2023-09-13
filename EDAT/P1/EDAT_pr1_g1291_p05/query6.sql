--A common retail analytics task is to analyze the orders to learn what products
--are often purchased together. Report the names of pairs of products that
--appear together in more than one “shopping cart” (i.e. order). The output
--should show the IDs of both products and the number of shopping carts in
--which they appear. Note that given two products with IDs, id1 and Id2, you
--should not consider the pairs (Id1, Id2) and (Id2, Id1) as different.


select p1.productcode, p2.productcode, count(*)                             -- Elements to be selected and printed
                                                                            -- (count(*) gives us the total amount of
                                                                            -- combinations of this products)

from products p1 join orderdetails od1 on p1.productcode = od1.productcode  -- In this lines we have established
	join products p2 on p1.productcode != p2.productcode                    -- the relations between the tables and they
	join orderdetails od2 on p2.productcode = od2.productcode               -- primary and foreign keys using joins and
	                                                                        -- on because they are not repeated

where p1.productcode > p2.productcode and od1.ordernumber = od2.ordernumber -- Two extra conditions to satisfy the
                                                                            -- exercise's result

group by p1.productcode, p2.productcode                                     -- To group the different results counted

having count(*) > 1                                                         -- Condition to show only those products
                                                                            -- that appear together more than once

order by count(*)                                                           -- We have ordered the results for being
                                                                            -- more aesthetic
