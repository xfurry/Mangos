DELETE FROM mangos_string WHERE entry BETWEEN 1600 AND 1636;
INSERT INTO mangos_string (entry, content_default) VALUES

-- Eastern Plaguelands
(1600,'|cffffff00Northpass Tower has been taken by the Horde!|r'),
(1601,'|cffffff00Northpass Tower has been taken by the Alliance!|r'),
(1602,'|cffffff00Crown Guard Tower has been taken by the Horde!|r'),
(1603,'|cffffff00Crown Guard Tower has been taken by the Alliance!|r'),
(1604,'|cffffff00Eastwall Tower has been taken by the Horde!|r'),
(1605,'|cffffff00Eastwall Tower has been taken by the Alliance!|r'),
(1606,'|cffffff00The Plaguewood Tower has been taken by the Horde!|r'),
(1607,'|cffffff00The Plaguewood Tower has been taken by the Alliance!|r'),

-- Hellfire Peninsula
(1608,'|cffffff00The Overlook has been taken by the Horde!|r'),
(1609,'|cffffff00The Overlook has been taken by the Alliance!|r'),
(1610,'|cffffff00The Stadium has been taken by the Horde!|r'),
(1611,'|cffffff00The Stadium has been taken by the Alliance!|r'),
(1612,'|cffffff00Broken Hill has been taken by the Horde!|r'),
(1613,'|cffffff00Broken Hill has been taken by the Alliance!|r'),

-- Zangarmarsh
(1614,'|cffffff00The Horde has taken control of the East Beacon!|r'),
(1615,'|cffffff00The Alliance has taken control of the East Beacon!|r'),
(1616,'|cffffff00The Horde has taken control of the West Beacon!|r'),
(1617,'|cffffff00The Alliance has taken control of the West Beacon!|r'),
(1618,'|cffffff00The Horde has taken control of both beacons!|r'),
(1619,'|cffffff00The Alliance has taken control of both beacons!|r'),
(1620,'|cffffff00The Horde Field Scout is now issuing battle standards.|r'),
(1621,'|cffffff00The Alliance Field Scout is now issuing battle standards.|r'),
(1622,'|cffffff00The Horde has taken control of Twin Spire Ruins!|r'),
(1623,'|cffffff00The Alliance has taken control of Twin Spire Ruins!|r'), -- TODO: Also send SMSG_PLAY_SOUND id 8173, from npc 15384 (npc id may be a parsing error, but unlikely)

-- Terokkar Forest
(1624,'|cffffff00The Horde has taken control of a Spirit Tower!|r'),
(1625,'|cffffff00The Alliance has taken control of a Spirit Tower!|r'),
(1626,'|cffffff00The Horde has lost control of a Spirit Tower!|r'),
(1627,'|cffffff00The Alliance has lost control of a Spirit Tower!|r'),
(1628,'|cffffff00The Horde has taken control of The Bone Wastes!|r'),
(1629,'|cffffff00The Alliance has taken control of The Bone Wastes!|r'),

-- Nagrand
(1630,'|cffffff00The Horde is gaining control of Halaa!|r'),
(1631,'|cffffff00The Alliance is gaining control of Halaa!|r'),
(1632,'|cffffff00The Horde has taken control of Halaa!|r'),
(1633,'|cffffff00The Alliance has taken control of Halaa!|r'),
(1634,'|cffffff00Halaa is defenseless!|r'),

-- Silithus
(1635,'|cffffff00The Horde has collected 200 silithyst!|r'),
(1636,'|cffffff00The Alliance has collected 200 silithyst!|r');
