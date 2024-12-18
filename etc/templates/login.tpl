<html lang="ru" data-theme="light">
    <head>
        <meta charset="UTF-8">
        <title>Login</title>
        <link rel="stylesheet" href="./authStyles.css">
    </head>
    <body>
            <form class="card" action="src/actions/login.php" method="post">
                <h2 class="login__header">Вход</h2>

                <div class="login__content">
                    {error}

                    <label for="email">
                        Email
                        <input
                            type="text"
                            id="email"
                            name="email"
                            <?php echo isAriaInvalidByName('email') ?>
                        >
                        <?php if (hasError('email')): ?>
                            <small> <?php echo getErrorText('email')?> </small>
                        <?php endif; ?>
                    </label>

                    <label for="password">
                        Пароль
                        <input
                            type="password"
                            id="password"
                            name="password"
                            <?php echo isAriaInvalidByName('password') ?>
                        >
                        <?php if (hasError('password')): ?>
                            <small> <?php echo getErrorText('password')?> </small>
                        <?php endif; ?>
                    </label>

                    <button
                        type="submit"
                        id="submit"
                        class="login__submit"
                    >Продолжить</button>
                </div>
                <p class="login__p">У меня еще нет <a class="login__reg" href="/register.php">аккаунта</a></p>
            </form>


        </body>
    </html>
