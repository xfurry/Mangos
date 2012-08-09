DELETE FROM `mangos_string` WHERE `entry` BETWEEN 10001 AND 10037;
INSERT INTO `mangos_string` (`entry`, `content_default`) VALUES

-- Silithus
(10001,'|cffffff00The Horde has collected 200 silithyst!|r'),
(10002,'|cffffff00The Alliance has collected 200 silithyst!|r'),

-- Eastern Plaguelands
(10003,'|cffffff00Northpass Tower has been taken by the Horde!|r'),
(10004,'|cffffff00Northpass Tower has been taken by the Alliance!|r'),
(10005,'|cffffff00Crown Guard Tower has been taken by the Horde!|r'),
(10006,'|cffffff00Crown Guard Tower has been taken by the Alliance!|r'),
(10007,'|cffffff00Eastwall Tower has been taken by the Horde!|r'),
(10008,'|cffffff00Eastwall Tower has been taken by the Alliance!|r'),
(10009,'|cffffff00The Plaguewood Tower has been taken by the Horde!|r'),
(10010,'|cffffff00The Plaguewood Tower has been taken by the Alliance!|r'),

-- Hellfire Peninsula
(10011,'|cffffff00The Overlook has been taken by the Horde!|r'),
(10012,'|cffffff00The Overlook has been taken by the Alliance!|r'),
(10013,'|cffffff00The Stadium has been taken by the Horde!|r'),
(10014,'|cffffff00The Stadium has been taken by the Alliance!|r'),
(10015,'|cffffff00Broken Hill has been taken by the Horde!|r'),
(10016,'|cffffff00Broken Hill has been taken by the Alliance!|r'),

-- Zangarmarsh
(10017,'|cffffff00The Horde has taken control of the East Beacon!|r'),
(10018,'|cffffff00The Alliance has taken control of the East Beacon!|r'),
(10019,'|cffffff00The Horde has taken control of the West Beacon!|r'),
(10020,'|cffffff00The Alliance has taken control of the West Beacon!|r'),
(10021,'|cffffff00The Horde has taken control of both beacons!|r'),
(10022,'|cffffff00The Alliance has taken control of both beacons!|r'),
(10023,'|cffffff00The Horde Field Scout is now issuing battle standards.|r'),
(10024,'|cffffff00The Alliance Field Scout is now issuing battle standards.|r'),
(10025,'|cffffff00The Horde has taken control of Twin Spire Ruins!|r'),
(10026,'|cffffff00The Alliance has taken control of Twin Spire Ruins!|r'), -- TODO: Also send SMSG_PLAY_SOUND id 8173, from npc 15384 (npc id may be a parsing error, but unlikely)

-- Terokkar Forest
(10027,'|cffffff00The Horde has taken control of a Spirit Tower!|r'),
(10028,'|cffffff00The Alliance has taken control of a Spirit Tower!|r'),
(10029,'|cffffff00The Horde has lost control of a Spirit Tower!|r'),
(10030,'|cffffff00The Alliance has lost control of a Spirit Tower!|r'),
(10031,'|cffffff00The Horde has taken control of The Bone Wastes!|r'),
(10032,'|cffffff00The Alliance has taken control of The Bone Wastes!|r'),

-- Nagrand
(10033,'|cffffff00The Horde is gaining control of Halaa!|r'),
(10034,'|cffffff00The Alliance is gaining control of Halaa!|r'),
(10035,'|cffffff00The Horde has taken control of Halaa!|r'),
(10036,'|cffffff00The Alliance has taken control of Halaa!|r'),
(10037,'|cffffff00Halaa is defenseless!|r');
