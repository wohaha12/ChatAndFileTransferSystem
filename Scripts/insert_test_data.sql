-- 插入测试数据
USE chat_file_transfer;

-- 插入测试用户
INSERT INTO t_user (username, password_hash, salt, nickname, avatar_url, status) VALUES
('admin', '8c6976e5b5410415bde908bd4dee15dfb167a9c873fc4bb8a81f6f2ab448a918', 'admin_salt', '管理员', '', 1),
('user1', '5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8', 'user1_salt', '用户1', '', 1),
('user2', '4b6e6e0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f', 'user2_salt', '用户2', '', 0);

-- 插入测试存储服务器
INSERT INTO t_storage_server (server_name, ip_address, port, status, capacity_total, capacity_used, weight) VALUES
('StorageServer1', '127.0.0.1', 9001, 1, 107374182400, 0, 1),
('StorageServer2', '127.0.0.1', 9002, 1, 107374182400, 0, 1);

-- 插入测试好友关系
INSERT INTO t_friend (user_id, friend_id) VALUES
(2, 3),
(3, 2);

-- 显示测试数据插入成功信息
SELECT 'Test data inserted successfully!' AS Message;
