-- MySQL dump 10.16  Distrib 10.1.26-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: 127.0.0.1    Database: Parking
-- ------------------------------------------------------
-- Server version	10.1.26-MariaDB-0+deb9u1

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `Active`
--

DROP TABLE IF EXISTS `Active`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Active` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rf_id` int(11) unsigned NOT NULL,
  `in_time` datetime NOT NULL,
  `in_number` tinyint(3) unsigned NOT NULL,
  `img` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=29 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Active`
--

LOCK TABLES `Active` WRITE;
/*!40000 ALTER TABLE `Active` DISABLE KEYS */;
INSERT INTO `Active` VALUES (1,12345678,'2018-04-09 15:09:38',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(2,3705,'2018-04-13 14:38:48',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(4,81041,'2018-04-13 14:41:08',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(5,0,'2018-04-13 15:10:17',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(9,1551160,'2018-04-13 16:53:00',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(10,1562008,'2018-04-13 16:55:00',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(11,1551768,'2018-04-13 16:55:40',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(13,2294528,'2018-04-13 17:10:19',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(15,26514,'2018-04-13 17:30:42',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(16,81043,'2018-04-13 17:30:45',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(19,28452,'2018-04-13 23:35:35',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(20,455856,'2018-04-13 23:36:17',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(22,57004,'2018-04-13 23:55:30',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(23,390502,'2018-04-13 23:55:37',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(24,1551164,'2018-04-13 23:55:46',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(25,652647,'2018-04-13 23:55:53',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(26,81483,'2018-04-13 23:56:18',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(27,775576,'2018-04-13 23:57:28',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg'),(28,1551152,'2018-04-15 03:46:26',1,'/home/bahman/Pictures/Parking/201846_21223.jpeg');
/*!40000 ALTER TABLE `Active` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `History`
--

DROP TABLE IF EXISTS `History`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `History` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `rf_id` int(11) unsigned NOT NULL,
  `in_time` datetime NOT NULL,
  `out_time` datetime DEFAULT NULL,
  `in_number` tinyint(3) unsigned NOT NULL,
  `out_number` tinyint(3) unsigned NOT NULL,
  `price` decimal(10,2) NOT NULL,
  `img` tinytext NOT NULL,
  `img_out` tinytext NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=12 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `History`
--

LOCK TABLES `History` WRITE;
/*!40000 ALTER TABLE `History` DISABLE KEYS */;
INSERT INTO `History` VALUES (3,1551152,'2018-04-13 17:30:57','2018-04-13 18:49:59',1,2,3000.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(4,1551152,'2018-04-13 21:09:33','2018-04-13 21:12:02',1,3,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(5,1551152,'2018-04-13 21:15:22','2018-04-14 03:16:28',5,7,7000.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(6,1551152,'2018-04-13 23:36:53','2018-04-14 00:13:44',1,1,99999999.99,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(7,1551152,'2018-04-14 06:50:07','2018-04-14 06:51:59',1,1,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(8,1551152,'2018-04-14 07:03:41','2018-04-14 07:04:13',1,1,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(9,1551152,'2018-04-14 07:20:34','2018-04-14 07:20:58',1,1,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(10,1551152,'2018-04-14 07:34:28','2018-04-14 07:34:56',1,1,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg'),(11,1551152,'2018-04-14 07:51:30','2018-04-14 07:51:58',1,1,0.00,'/home/bahman/Pictures/Parking/201846_21223.jpeg','/home/bahman/Pictures/Parking/201846_21223.jpeg');
/*!40000 ALTER TABLE `History` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `Price`
--

DROP TABLE IF EXISTS `Price`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `Price` (
  `price_per_hour` double unsigned NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Price`
--

LOCK TABLES `Price` WRITE;
/*!40000 ALTER TABLE `Price` DISABLE KEYS */;
INSERT INTO `Price` VALUES (3000);
/*!40000 ALTER TABLE `Price` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Dumping events for database 'Parking'
--

--
-- Dumping routines for database 'Parking'
--
/*!50003 DROP PROCEDURE IF EXISTS `MoveToHistory` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `MoveToHistory`(IN `b_rf_id` INT, IN `b_out_number` TINYINT, IN `b_img_out` TINYTEXT)
    MODIFIES SQL DATA
BEGIN

DECLARE b_id INT;
DECLARE b_in_time DATETIME;
DECLARE b_in_number TINYINT;
DECLARE b_img TINYTEXT;

DECLARE b_price DOUBLE;

DECLARE b_last_insert_id INT;

SELECT  `id`, `in_time`, `in_number`, `img` 
FROM `Parking`.`Active` 
WHERE b_rf_id=`rf_id` LIMIT 1
INTO b_id, b_in_time, b_in_number, b_img;

SELECT `price_per_hour` 
FROM `Parking`.`Price` 
INTO b_price;

INSERT INTO `Parking`.`History` (`rf_id`, `in_time`, `out_time`, `in_number`, `out_number`, `price`, `img`, `img_out`) 
VALUES (b_rf_id, b_in_time, SYSDATE(), b_in_number, b_out_number, TIMESTAMPDIFF(MINUTE,b_in_time,SYSDATE())*b_price/60, b_img, b_img_out);

SET b_last_insert_id = LAST_INSERT_ID();

DELETE FROM `Parking`.`Active` 
WHERE  `id`=b_id;

SELECT `in_time`, `out_time`, `in_number`, `price` 
FROM `Parking`.`History` 
WHERE `id` = b_last_insert_id;

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!50003 DROP PROCEDURE IF EXISTS `RegisterCar` */;
/*!50003 SET @saved_cs_client      = @@character_set_client */ ;
/*!50003 SET @saved_cs_results     = @@character_set_results */ ;
/*!50003 SET @saved_col_connection = @@collation_connection */ ;
/*!50003 SET character_set_client  = utf8mb4 */ ;
/*!50003 SET character_set_results = utf8mb4 */ ;
/*!50003 SET collation_connection  = utf8mb4_general_ci */ ;
/*!50003 SET @saved_sql_mode       = @@sql_mode */ ;
/*!50003 SET sql_mode              = 'NO_AUTO_VALUE_ON_ZERO' */ ;
DELIMITER ;;
CREATE DEFINER=`root`@`%` PROCEDURE `RegisterCar`(IN `b_rfid` INT, IN `b_in_number` TINYINT, IN `b_img` TINYTEXT)
    MODIFIES SQL DATA
BEGIN

INSERT INTO `Parking`.`Active` (`rf_id`, `in_time`, `in_number`, `img`) 
VALUES (b_rfid,SYSDATE(), b_in_number, b_img);

END ;;
DELIMITER ;
/*!50003 SET sql_mode              = @saved_sql_mode */ ;
/*!50003 SET character_set_client  = @saved_cs_client */ ;
/*!50003 SET character_set_results = @saved_cs_results */ ;
/*!50003 SET collation_connection  = @saved_col_connection */ ;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-04-15 14:19:10
