import java.sql.*;

public class JDBCExample {
    private static final String URL = "jdbc:mysql://localhost:3306/test_db?useSSL=false";
    private static final String USER = "root";
    private static final String PASSWORD = "your_password";

    public static void main(String[] args) {
        try (Connection connection = DriverManager.getConnection(URL, USER, PASSWORD)) {
            if (connection != null) {
                System.out.println("Connected to the database!");

                // Create
                String insertSQL = "INSERT INTO employees (name, email, department) VALUES (?, ?, ?)";
                try (PreparedStatement preparedStatement = connection.prepareStatement(insertSQL)) {
                    preparedStatement.setString(1, "Dhiraj Kumar");
                    preparedStatement.setString(2, "dhirajkkumar@example.com");
                    preparedStatement.setString(3, "Marketing");
                    preparedStatement.executeUpdate();
                    System.out.println("Inserted record into the table...");
                }

                // Read
                String selectSQL = "SELECT * FROM employees";
                try (Statement statement = connection.createStatement();
                     ResultSet resultSet = statement.executeQuery(selectSQL)) {
                    while (resultSet.next()) {
                        System.out.println("ID: " + resultSet.getInt("id"));
                        System.out.println("Name: " + resultSet.getString("name"));
                        System.out.println("Email: " + resultSet.getString("email"));
                        System.out.println("Department: " + resultSet.getString("department"));
                    }
                }

                // Update
//                String updateSQL = "UPDATE employees SET email = ? WHERE name = ?";
//                try (PreparedStatement preparedStatement = connection.prepareStatement(updateSQL)) {
//                    preparedStatement.setString(1, "john.newemail@example.com");
//                    preparedStatement.setString(2, "John Doe");
//                    preparedStatement.executeUpdate();
//                    System.out.println("Updated record in the table...");
//                }
//
//                // Delete
//                String deleteSQL = "DELETE FROM employees WHERE name = ?";
//                try (PreparedStatement preparedStatement = connection.prepareStatement(deleteSQL)) {
//                    preparedStatement.setString(1, "John Doe");
//                    preparedStatement.executeUpdate();
//                    System.out.println("Deleted record from the table...");
//                }
            } else {
                System.out.println("Failed to connect to the database.");
            }
        } catch (SQLException e) {
            e.printStackTrace();
        }
    }
}
