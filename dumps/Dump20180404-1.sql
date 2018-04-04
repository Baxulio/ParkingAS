-- MySQL dump 10.16  Distrib 10.1.26-MariaDB, for debian-linux-gnu (x86_64)
--
-- Host: localhost    Database: Parking
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
) ENGINE=InnoDB AUTO_INCREMENT=8 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `Active`
--

LOCK TABLES `Active` WRITE;
/*!40000 ALTER TABLE `Active` DISABLE KEYS */;
INSERT INTO `Active` VALUES (4,1,'2018-03-15 21:08:50',3,'3'),(6,121,'2018-03-15 23:35:02',4,'weq'),(7,123,'2018-03-15 23:35:19',4,'qwe');
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
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `History`
--

LOCK TABLES `History` WRITE;
/*!40000 ALTER TABLE `History` DISABLE KEYS */;
INSERT INTO `History` VALUES (1,21,'2018-03-15 03:25:44','2018-03-15 03:25:45',4,3,23.50,'2\0'),(2,123,'2018-03-15 02:37:51','2018-03-15 02:37:58',4,5,123.23,''),(7,1111,'2018-03-15 02:54:14','2018-03-15 05:26:12',3,5,5033.33,''),(8,1111,'2018-03-15 02:54:14','2018-03-15 20:56:41',3,5,36066.67,'//asd.jpg'),(9,1111,'2018-03-15 02:54:14','2018-03-15 20:57:41',3,5,36100.00,'//asd.jpg'),(10,1111,'2018-03-15 02:54:14','2018-03-15 21:03:43',3,5,36300.00,'//asd.jpg'),(11,1111,'2018-03-15 02:54:14','2018-03-15 21:07:45',3,5,36433.33,'//asd.jpg'),(12,1,'2018-03-15 21:08:41','2018-03-15 21:09:09',4,3,0.00,''),(13,3,'2018-03-15 21:11:11','2018-03-15 21:14:03',11,5,66.67,'11');
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
INSERT INTO `Price` VALUES (2000);
/*!40000 ALTER TABLE `Price` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2018-04-04 23:47:03
