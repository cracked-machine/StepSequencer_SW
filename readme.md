# Step Sequencer Project

For installation see [here](setup.md)

For building see [here](building.md)

[note: You can stick notes on diagrams too!{bg:cornsilk}]
[Customer]<>1-orders 0..*>[Order]
[Order]++*-*>[LineItem]
[Order]-1>[DeliveryMethod]
[Order]*-*>[Product|EAN_Code|promo_price()]
[Category]<->[Product]
[DeliveryMethod]^[National]
[DeliveryMethod]^[International]
