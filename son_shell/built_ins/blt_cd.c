/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   blt_cd.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sadinc <sadinc@student.42kocaeli.com.tr    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 11:22:50 by sadinc            #+#    #+#             */
/*   Updated: 2025/08/10 15:16:26 by sadinc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "built_in.h"
# include <stdio.h>
# include <unistd.h>

static int	count_args(char **args)
{
   int	count;

   count = 0;
   while (args[count])
   	count++;
   return (count);
}

static int	cd_to_home(t_data *data)
{
   char	*home;

   home = find_value_by_key(data, "HOME");
   if (home == NULL)
   {
   	write(2, "minishell: cd: HOME not set\n", 28);
	   free(home);
   	return (1);
   }
   if (chdir(home) != 0)
   {
   	perror("minishell: cd");
	   free(home);
   	return (1);
   }
   free(home);
   return (0);
}

int	builtin_cd(t_data *data, char **args)
{
   int	arg_count;

   arg_count = count_args(args);
   if (arg_count > 2)
   {
   	write(2, "minishell: cd: too many arguments\n", 34);
   	return (1);
   }
   if (!args[1])
   	return (cd_to_home(data));
   if (chdir(args[1]) != 0)
   {
   	perror("minishell: cd");
   	return (1);
   }
   return (0);
}