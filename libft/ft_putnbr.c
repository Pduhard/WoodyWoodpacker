/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pduhard- <pduhard-@student.le-101.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/02/27 02:15:53 by pduhard-          #+#    #+#             */
/*   Updated: 2020/02/27 02:15:54 by pduhard-         ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr(int n)
{
	char	nb;
	int		neg;

	neg = 0;
	if (n == -2147483648)
	{
		if (write(1, "-214748364", 10) == -1)
			return ;
		n = 8;
	}
	if (n < 0)
	{
		neg = 1;
		if (write(1, "-", 1) == -1)
			return ;
		ft_putnbr(-n);
	}
	if (n > 9)
		ft_putnbr(n / 10);
	nb = (n % 10) + '0';
	if (neg != 1 && write(1, &nb, 1) == -1)
		return ;
}
